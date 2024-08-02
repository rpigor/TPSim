#include "Simulator.hpp"
#include "VCD.hpp"
#include <iostream>
#include <algorithm>
#include <set>
#include <ctime>

using namespace boost;

std::string toString(tribool value) {
    if (indeterminate(value)) {
        return "x";
    } else if (value) {
        return "1";
    } else {
        return "0";
    }
}

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

    // init transaction list
    unsigned long stimuliTime = 0;
    std::vector<Transaction> transactionList;
    for (const auto& inputName : module.inputs) {
        std::vector<boost::tribool> inputVector = stimuli.at(inputName);
        Transaction prev_transaction{inputName, 0.0, !inputVector[0], 0};
        for (auto tb : inputVector) {
            if (prev_transaction.value == tb) {
                stimuliTime += cfg.clockPeriod;
                continue;
            }

            Transaction t{inputName, 0.0, tb, stimuliTime};
            transactionList.push_back(t);

            prev_transaction = t;
            stimuliTime += cfg.clockPeriod;
        }
        stimuliTime = 0;
    }

    // simulation loop
    unsigned long prevTime = (std::min_element(transactionList.begin(), transactionList.end(), [](Transaction i, Transaction j) { return i.tick < j.tick; }))->tick;
    VCDBuffer sameTimeTransactions;
    while (!transactionList.empty()) {
        auto it = std::min_element(transactionList.begin(), transactionList.end(), [](Transaction i, Transaction j) { return i.tick < j.tick; });
        Transaction t = *it;
        transactionList.erase(it);

        if (t.tick > cfg.timeLimit) {
            break;
        }

        wireStates[t.wire] = t.value;

        // handles gates affected by the transaction
        for (auto& g : module.gates) {
            Cell cell = lib.at(g.cell);

            auto inputIt = g.net2input.find(t.wire);
            bool affectsGateInput = inputIt != g.net2input.end();
            if (!affectsGateInput) {
                continue;
            }

            std::string inputPin = inputIt->second;
            std::vector<tribool> inputStates;
            for (const std::string& in : cell.inputs) {
                if (in == inputPin) {
                    inputStates.push_back(t.value);
                    continue;
                }

                std::string inWire = g.input2net.at(in);
                inputStates.push_back(wireStates.at(inWire));
            }

            // compute new transaction
            std::string outputPin = cell.outputs[0]; // only a single output is supported!
            std::string outputWire = g.output2net.at(outputPin);
            auto func = getCellOutputFunction(cell.name, outputPin);
            tribool result = func(inputStates);
            if (wireStates.at(outputWire) == result) {
                continue;
            }

            double outputCap = computeOutputCapacitance(outputWire, result);
            Arc arc{inputPin, outputPin};
            double delay = computeDelay(cell.name, arc, result, t.inputSlope, outputCap, cfg.allowExtrapolation);
            double inputSlope = computeOutputSlope(cell.name, arc, result, t.inputSlope, outputCap, cfg.allowExtrapolation);
            unsigned long resultingTick = t.tick + timeToTick(delay, cell.timeUnit, cfg.timescale);

            transactionList.push_back(Transaction{outputWire, inputSlope, result, resultingTick});
        }

        if (t.tick != prevTime) {
            vcd.printVarDumpBuffer(sameTimeTransactions);
        }

        sameTimeTransactions.insert(t);
        prevTime = t.tick;
    }

    vcd.printVarDumpBuffer(sameTimeTransactions);
}