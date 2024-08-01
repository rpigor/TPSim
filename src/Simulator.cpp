#include "Simulator.hpp"
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

VCDBuffer::VCDBuffer() : tick(-1) { }

void VCDBuffer::insert(const Transaction& t) {
    if (tick != t.tick) {
        buffer.clear();
        tick = t.tick;
    }
    buffer[t.wire].insert(t.value);
}

void VCDBuffer::clear() {
    buffer.clear();
    tick = -1;
}

void VCDBuffer::printVCD(std::ostream& os, const std::unordered_map<std::string, char>& wireIdMap) const {
    os << "#" << tick << "\n";
    for (auto& tupIt : buffer) {
        if (tupIt.second.size() == 1) {
            os << toString(*tupIt.second.begin()) << wireIdMap.at(tupIt.first) << "\n";
        }
        else {
            auto tbIt = std::find_if(tupIt.second.begin(), tupIt.second.end(), [](boost::tribool tb) { return !indeterminate(tb); });
            if (tbIt != tupIt.second.end()) {
                os << toString(*tbIt) << wireIdMap.at(tupIt.first) << "\n";
            }
            else {
                os << "x" << wireIdMap.at(tupIt.first) << "\n";
            }
        }
    }
}

// TODO: implement configurable time unit instead of hard coding it
unsigned long Simulator::toTick(double time) const {
    return static_cast<unsigned long>(time*1e3);
}

BooleanFunction Simulator::getCellOutputFunction(const std::string& cellName, const std::string& output) const {
    Cell cell = parser.lib.at(cellName);
    auto outIt = std::find(cell.outputs.begin(), cell.outputs.end(), output);
    unsigned int outIdx = std::distance(cell.outputs.begin(), outIt);
    return boost::apply_visitor(BooleanFunctionVisitor(), cellOutputExpressions.at(cell.name)[outIdx]);
}

double Simulator::computeOutputCapacitance(const std::string& outputWire, boost::tribool newState) const {
    double outputCap = 0.0;
    for (auto& g : parser.module.gates) {
        Cell cell = parser.lib.at(g.cell);

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

    auto slopeMatrix = newState ? parser.lib.at(cellName).delayAndSlope.at(arc).riseOutputSlope : parser.lib.at(cellName).delayAndSlope.at(arc).fallOutputSlope;
    auto inputSlopeVec = parser.lib.at(cellName).delayAndSlope.at(arc).inputSlope;
    auto outputCapacitanceVec = parser.lib.at(cellName).delayAndSlope.at(arc).outputCapacitance;

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

    auto delayMatrix = newState ? parser.lib.at(cellName).delayAndSlope.at(arc).riseDelay : parser.lib.at(cellName).delayAndSlope.at(arc).fallDelay;
    auto inputSlopeVec = parser.lib.at(cellName).delayAndSlope.at(arc).inputSlope;
    auto outputCapacitanceVec = parser.lib.at(cellName).delayAndSlope.at(arc).outputCapacitance;

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

Simulator::Simulator(const VerilogParser& parser, std::ostream& os) : parser(parser), os(os) {
    for (auto& w : parser.module.wires) {
        wireStates[w] = indeterminate;
    }

    for (auto& w : parser.module.inputs) {
        wireStates[w] = indeterminate;
    }

    for (auto& w : parser.module.outputs) {
        wireStates[w] = indeterminate;
    }

    BooleanParser<std::string::iterator> boolParser;
    for (auto& t : parser.lib) {
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

Simulator::Simulator(const VerilogParser& parser) : Simulator(parser, std::cout) { }

void Simulator::simulate(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, unsigned long timeLimit) {
    const unsigned int periodTime = 10000;

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    os  << "$version Simulator $end\n"
        << "$date " << std::put_time(&tm, "%d/%m/%Y %T") << " $end\n"
        << "$timescale 1ps $end\n"
        << "$scope module " << parser.module.name << " $end\n";

    std::unordered_map<std::string, char> wireIdMap;
    unsigned short wireId = 33;
    for (auto& t : wireStates) {
        char charId = static_cast<char>(wireId++);
        wireIdMap[t.first] = charId;
        os << "$var wire 1 " << charId << " " << t.first << " $end\n";
    }

    os  << "$upscope $end\n"
        << "$enddefinitions $end\n"
        << "$dumpvars\n";

    // init transaction list
    unsigned long stimuliTime = 0;
    std::vector<Transaction> transactionList;
    for (const auto& inputName : parser.module.inputs) {
        std::vector<boost::tribool> inputVector = stimuli.at(inputName);
        Transaction prev_transaction{inputName, 0.0, !inputVector[0], 0};
        for (auto tb : inputVector) {
            if (prev_transaction.value == tb) {
                stimuliTime += periodTime;
                continue;
            }

            Transaction t{inputName, 0.0, tb, stimuliTime};
            transactionList.push_back(t);

            prev_transaction = t;
            stimuliTime += periodTime;
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

        if (t.tick > timeLimit) {
            break;
        }

        wireStates[t.wire] = t.value;

        // handles gates affected by the transaction
        for (auto& g : parser.module.gates) {
            Cell cell = parser.lib.at(g.cell);

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
            std::string outputPin = cell.outputs[0];
            std::string outputWire = g.output2net.at(outputPin);
            auto func = getCellOutputFunction(cell.name, outputPin);
            tribool result = func(inputStates);
            if (wireStates.at(outputWire) == result) {
                continue;
            }

            double outputCap = computeOutputCapacitance(outputWire, result);
            Arc arc{inputPin, outputPin};
            double delay = computeDelay(cell.name, arc, result, t.inputSlope, outputCap, true);
            double inputSlope = computeOutputSlope(cell.name, arc, result, t.inputSlope, outputCap, true);
            unsigned long resultingTick = t.tick + toTick(delay);

            transactionList.push_back(Transaction{outputWire, inputSlope, result, resultingTick});
        }

        if (t.tick != prevTime) {
            sameTimeTransactions.printVCD(os, wireIdMap);
        }

        sameTimeTransactions.insert(t);
        prevTime = t.tick;
    }

    sameTimeTransactions.printVCD(os, wireIdMap);
}