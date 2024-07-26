#pragma once

#include "VerilogParser.hpp"
#include "BooleanParser.hpp"
#include <boost/logic/tribool.hpp>
#include <string>
#include <vector>
#include <unordered_map>

struct Transaction {
    std::string wire;
    boost::tribool value;
    long int tick;

    bool operator<(const Transaction& t) const {
        return std::tie(wire, value.value, tick) < std::tie(t.wire, t.value.value, t.tick);
    }
};

class Simulator {
private:

    const VerilogParser& parser;
    std::unordered_map<std::string, boost::tribool> wireStates;
    std::unordered_map<std::string, std::vector<Expression>> cellOutputExpressions;
    std::ostream& os;

public:

    Simulator() = delete;
    Simulator(const VerilogParser& parser);
    Simulator(const VerilogParser& parser, std::ostream& os);

    void simulate(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli);

private:

    BooleanFunction getCellOutputFunction(const std::string& cellName, const std::string& output) const;
    std::string toString(boost::tribool) const;
    std::vector<bool> toBoolVector(const std::vector<boost::tribool>& triboolVec) const;

};