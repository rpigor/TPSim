#pragma once

#include "VerilogParser.hpp"
#include "BooleanParser.hpp"
#include <boost/logic/tribool.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <unordered_map>

std::string toString(boost::tribool);

struct TriboolComp {
    bool operator()(boost::tribool tb1, boost::tribool tb2)  const {
        return tb1.value < tb2.value;
    }
};

struct Transaction {
    std::string wire;
    boost::tribool value;
    long int tick;

    bool operator<(const Transaction& t) const {
        return std::tie(wire, value.value, tick) < std::tie(t.wire, t.value.value, t.tick);
    }
};

class VCDBuffer {
private:

    long int tick;
    std::unordered_map<std::string, std::set<boost::tribool, TriboolComp>> buffer;

public:

    VCDBuffer();
    void insert(const Transaction& t);
    void clear();
    void printVCD(std::ostream& os, const std::unordered_map<std::string, char>& wireIdMap) const;

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

};