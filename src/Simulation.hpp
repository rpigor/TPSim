#pragma once

#include "VerilogParser.hpp"
#include "BooleanParser.hpp"
#include "StimuliParser.hpp"
#include "CellLibrary.hpp"
#include <boost/logic/tribool.hpp>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

struct SimulationConfig {
    const std::string& timescale;
    unsigned long clockPeriod;
    double stimuliSlope;
    double outputCapacitance;
    unsigned long timeLimit;
    bool allowExtrapolation;
    std::filesystem::path powerReportFile;
};

class Simulation {
private:

    const Module& module;
    const CellLibrary& lib;
    Stimuli stim;
    SimulationConfig cfg;

    std::unordered_map<std::string, boost::tribool> wireStates;
    std::unordered_map<std::string, std::vector<Expression>> cellOutputExpressions;

public:

    Simulation() = delete;
    Simulation(const Module& module, const CellLibrary& lib, const Stimuli& stim, const SimulationConfig& cfg);

    void run(std::ostream& os);

private:

    void printSimulationHeader(std::ostream& os, const SimulationConfig& cfg) const;
    void printSimulationProgress(std::ostream& os, unsigned long tick, const std::string& tickUnit, char animationChar) const;
    void printSimulationFooter(std::ostream& os, const std::chrono::steady_clock::time_point& startTime) const;
    boost::tribool evaluateCellOutput(const std::string& cellName, const std::string& output, const std::vector<boost::tribool>& input) const;
    double getInputStateLeakagePower(const std::string& cellName, const std::vector<boost::tribool>& inputState) const;
    double computeOutputCapacitance(const std::string& outputWire, boost::tribool newState, double defaultOutputCapacitance) const;

};