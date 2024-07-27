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

BooleanFunction Simulator::getCellOutputFunction(const std::string& cellName, const std::string& output) const {
    Cell cell = parser.lib.at(cellName);
    auto outIt = std::find(cell.outputs.begin(), cell.outputs.end(), output);
    unsigned int outIdx = std::distance(cell.outputs.begin(), outIt);
    return boost::apply_visitor(BooleanFunctionVisitor(), cellOutputExpressions.at(cell.name)[outIdx]);
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
                throw std::invalid_argument("\'" + boolFunc + "\' is not a valid boolean expression.");
            }
            cellOutputExpressions[cellName].push_back(result);
        }
    }
}

Simulator::Simulator(const VerilogParser& parser) : Simulator(parser, std::cout) { }

void Simulator::simulate(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli) {
    const int periodTick = 10;
    const int delay = 1;
    long int stimuli_tick = 0;

    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    os  << "$version Simulator $end\n"
        << "$date " << std::put_time(&tm, "%Y/%m/%d %T") << " $end\n"
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
    std::vector<Transaction> transactionList;
    for (const auto& inputName : parser.module.inputs) {
        std::vector<boost::tribool> inputVector = stimuli.at(inputName);
        Transaction prev_transaction{inputName, !inputVector[0], 0};
        for (auto tb : inputVector) {
            if (prev_transaction.value == tb) {
                stimuli_tick += periodTick;
                continue;
            }

            Transaction t{inputName, tb, stimuli_tick};
            transactionList.push_back(t);

            prev_transaction = t;
            stimuli_tick += periodTick;
        }
        stimuli_tick = 0;
    }

    // simulation loop
    long int prevTime = (std::min_element(transactionList.begin(), transactionList.end(), [](Transaction i, Transaction j) { return i.tick < j.tick; }))->tick;
    VCDBuffer sameTimeTransactions;
    while (!transactionList.empty()) {
        auto it = std::min_element(transactionList.begin(), transactionList.end(), [](Transaction i, Transaction j) { return i.tick < j.tick; });
        Transaction t = *it;
        transactionList.erase(it);

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

            std::string outputPin = cell.outputs[0];
            std::string outputWire = g.output2net.at(outputPin);
            auto func = getCellOutputFunction(cell.name, outputPin);
            tribool result = func(inputStates);

            if (wireStates.at(outputWire) == result) {
                continue;
            }

            transactionList.push_back(Transaction{outputWire, result, t.tick+delay});
        }

        if (t.tick != prevTime) {
            sameTimeTransactions.printVCD(os, wireIdMap);
        }

        sameTimeTransactions.insert(t);
        prevTime = t.tick;
    }

    sameTimeTransactions.printVCD(os, wireIdMap);
}