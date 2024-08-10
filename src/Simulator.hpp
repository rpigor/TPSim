#pragma once

#include "VerilogParser.hpp"
#include "BooleanParser.hpp"
#include <boost/logic/tribool.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

struct SimulationConfig {
    const std::string& timescale;
    unsigned long clockPeriod;
    unsigned long timeLimit;
    bool allowExtrapolation;
};

class Simulator {
private:

    const Module& module;
    const std::unordered_map<std::string, Cell>& lib;
    std::unordered_map<std::string, boost::tribool> wireStates;
    std::unordered_map<std::string, std::vector<Expression>> cellOutputExpressions;

public:

    Simulator() = delete;
    Simulator(const Module& module, const std::unordered_map<std::string, Cell>& lib);

    void simulate(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, const SimulationConfig& cfg, std::ostream& os);

private:

    unsigned long timeToTick(double time, const std::string& timeUnit, const std::string& tickUnit) const;
    BooleanFunction getCellOutputFunction(const std::string& cellName, const std::string& output) const;
    double computeOutputCapacitance(const std::string& outputWire, boost::tribool newState) const;

};