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

struct Transaction {
    std::string wire;
    double inputSlope;
    boost::tribool value;
    unsigned long tick;

    bool operator<(const Transaction& t) const {
        return std::tie(wire, inputSlope, value.value, tick) < std::tie(t.wire, t.inputSlope, t.value.value, t.tick);
    }
};

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
    unsigned long unitScale(const std::string& unit) const;
    BooleanFunction getCellOutputFunction(const std::string& cellName, const std::string& output) const;
    double computeOutputCapacitance(const std::string& outputWire, boost::tribool newState) const;
    double computeOutputSlope(const std::string& cellName, const Arc& arc, boost::tribool newState, double inputSlope, double outputCapacitance, bool extrapolate) const;
    double computeDelay(const std::string& cellName, const Arc& arc, boost::tribool newState, double inputSlope, double outputCapacitance, bool extrapolate) const;
    double bilinearInterpolate(double x, double y, const std::vector<double>& xVec, const std::vector<double>& yVec, const std::vector<std::vector<double>>& zMatrix, bool extrapolate) const;
    std::tuple<std::vector<double>::size_type, std::vector<double>::size_type> neighboringIdxs(double value, const std::vector<double>& valuesVec, bool extrapolate) const;
    double interpolate(double x, double x1, double x2, double y1, double y2) const;

};