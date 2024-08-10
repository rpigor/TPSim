#include "Simulator.hpp"
#include "EventQueue.hpp"
#include "Estimator.hpp"
#include "VCD.hpp"
#include "Units.hpp"
#include <iostream>
#include <algorithm>
#include <ctime>

using namespace boost;

Simulator::Simulator(const Module& module, const std::unordered_map<std::string, Cell>& lib)
: module(module), lib(lib) {
    for (auto& w : module.wires) {
        wireStates[w] = indeterminate;
    }

    for (auto& w : module.inputs) {
        wireStates[w] = indeterminate;
    }

    for (auto& w : module.outputs) {
        wireStates[w] = indeterminate;
    }

    BooleanParser<std::string::iterator> boolParser;
    for (auto& t : lib) {
        std::string cellName = t.first;
        Cell cell = t.second;
        for (unsigned int outIdx = 0; outIdx < cell.bitFunctions.size(); outIdx++) {
            std::string boolFunc = cell.bitFunctions[outIdx];
            auto firstIt(std::begin(boolFunc)), lastIt(std::end(boolFunc));

            Expression result;
            bool ok = boost::spirit::qi::phrase_parse(firstIt, lastIt, boolParser, boost::spirit::qi::space, result);
            if (!ok) {
                throw std::runtime_error("\'" + boolFunc + "\' is not a valid boolean expression.");
            }
            cellOutputExpressions[cellName].push_back(result);
        }
    }
}

unsigned long Simulator::timeToTick(double time, const std::string& timeUnit, const std::string& tickUnit) const {
    unsigned long factor = Units::unitScale(tickUnit) / Units::unitScale(timeUnit);
    return static_cast<unsigned long>(time*factor);
}

BooleanFunction Simulator::getCellOutputFunction(const std::string& cellName, const std::string& output) const {
    Cell cell = lib.at(cellName);
    auto outIt = std::find(cell.outputs.begin(), cell.outputs.end(), output);
    unsigned int outIdx = std::distance(cell.outputs.begin(), outIt);
    return boost::apply_visitor(BooleanFunctionVisitor(), cellOutputExpressions.at(cell.name)[outIdx]);
}

double Simulator::computeOutputCapacitance(const std::string& outputWire, boost::tribool newState) const {
    double outputCap = 0.0;
    for (auto& g : module.gates) {
        Cell cell = lib.at(g.cell);

        auto inputIt = g.net2input.find(outputWire);
        bool affectsGateInput = inputIt != g.net2input.end();
        if (!affectsGateInput) {
            continue;
        }
        outputCap += cell.pinCapacitance.at(inputIt->second);
    }

    return outputCap;
}

void Simulator::simulate(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, const SimulationConfig& cfg, std::ostream& os) {
    std::vector<std::string> wires;
    for (auto& t : wireStates) {
        wires.push_back(t.first);
    }
    VCDFormatter vcd(os, wires);

    vcd.printHeader(cfg.timescale);
    vcd.printDefinitions(module.name);
    vcd.printVarDumpInit();

    // initiailize event queue with external stimuli
    EventQueue events(stimuli, module.inputs, cfg.clockPeriod);

    // simulation loop
    unsigned long prevTime = events.top().tick;
    VCDBuffer sameTickEvs;
    while (!events.empty()) {
        Event ev = events.top();
        events.pop();

        if (ev.tick > cfg.timeLimit) {
            break;
        }

        wireStates[ev.wire] = ev.value;

        // handles gates affected by the event
        for (auto& g : module.gates) {
            Cell cell = lib.at(g.cell);

            auto inputIt = g.net2input.find(ev.wire);
            bool affectsGateInput = inputIt != g.net2input.end();
            if (!affectsGateInput) {
                continue;
            }

            std::string inputPin = inputIt->second;
            std::vector<tribool> inputStates;
            for (const std::string& in : cell.inputs) {
                if (in == inputPin) {
                    inputStates.push_back(ev.value);
                    continue;
                }

                std::string inWire = g.input2net.at(in);
                inputStates.push_back(wireStates.at(inWire));
            }

            // compute new event
            std::string outputPin = cell.outputs[0]; // only a single output is supported!
            std::string outputWire = g.output2net.at(outputPin);
            auto func = getCellOutputFunction(cell.name, outputPin);
            tribool result = func(inputStates);
            if (wireStates.at(outputWire) == result) {
                continue;
            }

            double outputCap = computeOutputCapacitance(outputWire, result);
            Arc arc{inputPin, outputPin};
            double delay = indeterminate(result) ? 0.0 : Estimator::estimate(lib.at(cell.name).delay, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
            double inputSlope = indeterminate(result) ? 0.0 : Estimator::estimate(lib.at(cell.name).outputSlope, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
            unsigned long resultingTick = ev.tick + timeToTick(delay, cell.timeUnit, cfg.timescale);

            events.push(Event{outputWire, inputSlope, result, resultingTick});
        }

        if (ev.tick != prevTime) {
            vcd.printVarDumpBuffer(sameTickEvs);
        }

        sameTickEvs.insert(ev);
        prevTime = ev.tick;
    }

    vcd.printVarDumpBuffer(sameTickEvs);
}