#include "Simulator.hpp"
#include "EventQueue.hpp"
#include "VCD.hpp"
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
    unsigned long factor = unitScale(tickUnit) / unitScale(timeUnit);
    return static_cast<unsigned long>(time*factor);
}

unsigned long Simulator::unitScale(const std::string& unit) const {
    switch (unit[0]) {
        case 'm': return 1e3;
        case 'u': return 1e6;
        case 'n': return 1e9;
        case 'p': return 1e12;
        case 'f': return 1e15;
        case 'a': return 1e18;
        default: return 1e0;
    }
    return 1e0;
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

double Simulator::computeOutputSlope(
                                        const std::string& cellName,
                                        const Arc& arc, boost::tribool newState,
                                        double inputSlope, double outputCapacitance,
                                        bool extrapolate
                                    ) const {
    if (indeterminate(newState)) {
        return 0.0;
    }

    auto slopeMatrix = newState ? lib.at(cellName).delayAndSlope.at(arc).riseOutputSlope : lib.at(cellName).delayAndSlope.at(arc).fallOutputSlope;
    auto inputSlopeVec = lib.at(cellName).delayAndSlope.at(arc).inputSlope;
    auto outputCapacitanceVec = lib.at(cellName).delayAndSlope.at(arc).outputCapacitance;

    double delay = bilinearInterpolate(inputSlope, outputCapacitance, inputSlopeVec, outputCapacitanceVec, slopeMatrix, extrapolate);
    return delay;

}

double Simulator::computeDelay(
                                    const std::string& cellName,
                                    const Arc& arc, boost::tribool newState,
                                    double inputSlope, double outputCapacitance,
                                    bool extrapolate
                                ) const {
    if (indeterminate(newState)) {
        return 0.0;
    }

    auto delayMatrix = newState ? lib.at(cellName).delayAndSlope.at(arc).riseDelay : lib.at(cellName).delayAndSlope.at(arc).fallDelay;
    auto inputSlopeVec = lib.at(cellName).delayAndSlope.at(arc).inputSlope;
    auto outputCapacitanceVec = lib.at(cellName).delayAndSlope.at(arc).outputCapacitance;

    double delay = bilinearInterpolate(inputSlope, outputCapacitance, inputSlopeVec, outputCapacitanceVec, delayMatrix, extrapolate);
    return delay;
}

double Simulator::bilinearInterpolate(
                                        double x, double y,
                                        const std::vector<double>& xVec, const std::vector<double>& yVec,
                                        const std::vector<std::vector<double>>& zMatrix,
                                        bool extrapolate
                                    ) const {
    auto [xLowerIdx, xUpperIdx] = neighboringIdxs(x, xVec, extrapolate);
    auto xLower = xVec.at(xLowerIdx);
    auto xUpper = xVec.at(xUpperIdx);

    auto [yLowerIdx, yUpperIdx] = neighboringIdxs(y, yVec, extrapolate);
    auto yLower = yVec.at(yLowerIdx);
    auto yUpper = yVec.at(yUpperIdx);

    double zLowerXLowerY = zMatrix.at(xLowerIdx).at(yLowerIdx);
    double zLowerXUpperY = zMatrix.at(xLowerIdx).at(yUpperIdx);
    double zUpperXLowerY = zMatrix.at(xUpperIdx).at(yLowerIdx);
    double zUpperXUpperY = zMatrix.at(xUpperIdx).at(yUpperIdx);

    double zLowerX = interpolate(y, yLower, yUpper, zLowerXLowerY, zLowerXUpperY);
    double zUpperX = interpolate(y, yLower, yUpper, zUpperXLowerY, zUpperXUpperY);
    double zItp = interpolate(x, xLower, xUpper, zLowerX, zUpperX);

    return zItp;
}

std::tuple<std::vector<double>::size_type, std::vector<double>::size_type> Simulator::neighboringIdxs(double value, const std::vector<double>& valuesVec, bool extrapolate) const {
    auto endIt = std::find_if(valuesVec.begin(), valuesVec.end(), [value](double v) { return value > v; });
    decltype(endIt) startIt;

    if (endIt == valuesVec.end()) {
        if (!extrapolate) {
            throw std::runtime_error("cannot find neighboring value " + std::to_string(value) + " (out of upper bound)");
        }
        endIt--;
    }
    else if (endIt == valuesVec.begin()) {
        if (!extrapolate) {
            throw std::runtime_error("cannot find neighboring value " + std::to_string(value) + " (out of lower bound)");
        }
        endIt++;
    }
    startIt = endIt - 1;

    return { std::distance(valuesVec.begin(), startIt), std::distance(valuesVec.begin(), endIt) };
}

double Simulator::interpolate(double x, double x1, double x2, double y1, double y2) const {
    return y1 + ((y2-y1)/(x2-x1))*(x-x1);
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
            double delay = computeDelay(cell.name, arc, result, ev.inputSlope, outputCap, cfg.allowExtrapolation);
            double inputSlope = computeOutputSlope(cell.name, arc, result, ev.inputSlope, outputCap, cfg.allowExtrapolation);
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