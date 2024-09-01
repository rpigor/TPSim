#include "Simulator.hpp"
#include "EventQueue.hpp"
#include "Power.hpp"
#include "Estimator.hpp"
#include "VCD.hpp"
#include "Units.hpp"
#include <iostream>
#include <algorithm>
#include <ctime>

using namespace boost;

Simulator::Simulator(const Module& module, const CellLibrary& lib)
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
    for (auto& t : lib.cells) {
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

BooleanFunction Simulator::getCellOutputFunction(const std::string& cellName, const std::string& output) const {
    Cell cell = lib.cells.at(cellName);
    auto outIt = std::find(cell.outputs.begin(), cell.outputs.end(), output);
    unsigned int outIdx = std::distance(cell.outputs.begin(), outIt);
    return boost::apply_visitor(BooleanFunctionVisitor(), cellOutputExpressions.at(cell.name)[outIdx]);
}

double Simulator::computeOutputCapacitance(const std::string& outputWire, boost::tribool newState, double defaultOutputCapacitance) const {
    int affectedGates = 0;
    double outputCap = 0.0;
    for (auto& g : module.gates) {
        Cell cell = lib.cells.at(g.cell);

        auto inputIt = g.net2input.find(outputWire);
        bool affectsGateInput = inputIt != g.net2input.end();
        if (!affectsGateInput) {
            continue;
        }
        outputCap += cell.pinCapacitance.at(inputIt->second);
        affectedGates++;
    }

    if (affectedGates == 0) { // last stage
        return defaultOutputCapacitance;
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
    EventQueue events(stimuli, module.inputs, cfg.clockPeriod, cfg.stimuliSlope);

    std::vector<Energy> energyVec;

    // simulation loop
    unsigned long prevTime = events.top().tick;
    double prevSlope = events.top().inputSlope;
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
            Cell cell = lib.cells.at(g.cell);

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

            // estimate Event parameters
            double outputCap = computeOutputCapacitance(outputWire, result, cfg.outputCapacitance);
            Arc arc{inputPin, outputPin};
            double delay = indeterminate(result) ? 0.0 : Estimator::estimate(lib.cells.at(cell.name).delay, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
            double inputSlope = indeterminate(result) ? 0.0 : Estimator::estimate(lib.cells.at(cell.name).outputSlope, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
            unsigned long resultingTick = ev.tick + Units::timeToTick(delay, cell.timeUnit, cfg.timescale);

            // estimate dynamic energy
            double internalEnergy = indeterminate(result) ? 0.0 : std::fabs(Estimator::estimate(lib.cells.at(cell.name).internalPower, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation));
            double internalEnergyScaled = internalEnergy / Units::unitScale(cell.internalPowerUnit);
            double switchingEnergy = outputCap*lib.vdd*lib.vdd / 2;
            double switchingEnergyScaled = switchingEnergy / Units::unitScale(cell.capacitanceUnit);
            if (wireStates.at(outputWire) == result) { // if output doesn't change, switching energy is not consumed
                switchingEnergy = 0.0;
            }
            unsigned long eventEndTick = Estimator::estimateEndTime(resultingTick, inputSlope, cell.timeUnit, cfg.timescale);
            energyVec.push_back({resultingTick, eventEndTick, internalEnergyScaled + switchingEnergyScaled, true});

            // estimate leakage energy
            if (ev.tick != 0) { // ignore first event
                std::string state;
                for (auto tb : inputStates) {
                    state.push_back(tb ? '1' : '0');
                }
                double leakagePower = lib.cells.at(cell.name).leakage.at(state);
                unsigned long startLeakTick = Estimator::estimateEndTime(prevTime, prevSlope, cell.timeUnit, cfg.timescale);
                double leakageInterval = Units::tickToTime(ev.tick - startLeakTick, cell.timeUnit, cfg.timescale); // from the end of the previous event to the start of the current event
                double leakageEnergy = leakagePower*leakageInterval / (Units::unitScale(cell.timeUnit)*Units::unitScale(cell.leakagePowerUnit));
                energyVec.push_back({startLeakTick, ev.tick, leakageEnergy, false});
            }

            if (wireStates.at(outputWire) == result) {
                continue;
            }
            events.push(Event{outputWire, inputSlope, result, resultingTick});
        }

        if (ev.tick != prevTime) {
            vcd.printVarDumpBuffer(sameTickEvs);
        }

        sameTickEvs.insert(ev);
        prevTime = ev.tick;
    }

    vcd.printVarDumpBuffer(sameTickEvs);

    /*
    for (auto& e : energyVec) {
        std::cout << e << std::endl;
    }

    double totalEnergy = Power::accumulateEnergy(energyVec);
    double dynEnergy = Power::accumulateDynamicEnergy(energyVec);
    double staticEnergy = Power::accumulateStaticEnergy(energyVec);
    double dynPct = 100 * (dynEnergy / totalEnergy);
    double staticPct = 100 - dynPct;
    std::cout << "Total energy:\t" << totalEnergy << std::endl;
    std::cout << "Dynamic energy:\t" << dynEnergy  << " (" << dynPct << "%)" << std::endl;
    std::cout << "Static energy:\t" << staticEnergy << " (" << staticPct << "%)" << std::endl;
    */
}