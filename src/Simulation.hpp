#pragma once

#include "VerilogParser.hpp"
#include "BooleanParser.hpp"
#include "StimuliParser.hpp"
#include "CellLibrary.hpp"
#include "Event.hpp"
#include "SimulationSubscriber.hpp"
#include <boost/logic/tribool.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>

struct SimulationConfig {
    const std::string& timescale;
    unsigned long clockPeriod;
    std::size_t wheelSize;
    double stimuliSlope;
    double outputCapacitance;
    unsigned long timeLimit;
    bool allowExtrapolation;
};

class SimulationOnBeginSubscriber;
class SimulationOnNewEventSubscriber;
class SimulationAfterHandlingEventSubscriber;
class SimulationOnEndSubscriber;

class Simulation {
private:

    const Module& module;
    const CellLibrary& lib;
    Stimuli stim;
    SimulationConfig cfg;

    std::unordered_map<std::string, boost::tribool> wireStates;
    std::unordered_map<std::string, std::vector<Expression>> cellOutputExpressions;

    std::vector<SimulationOnBeginSubscriber*> beginSubscribers;
    std::vector<SimulationOnNewEventSubscriber*> newEventSubscribers;
    std::vector<SimulationAfterHandlingEventSubscriber*> afterEventSubscribers;
    std::vector<SimulationOnEndSubscriber*> endSubscribers;

public:

    Simulation() = delete;
    Simulation(const Module& module, const CellLibrary& lib, const Stimuli& stim, const SimulationConfig& cfg);

    void run();

    const Module& getModule() const;
    const CellLibrary& getLibrary() const;
    Stimuli getStimuli() const;
    SimulationConfig getConfiguration() const;

    // Observer pattern
    void hookOnBeginSubscriber(SimulationOnBeginSubscriber* subscriber);
    void hookOnNewEventSubscriber(SimulationOnNewEventSubscriber* subscriber);
    void hookAfterHandlingEventSubscriber(SimulationAfterHandlingEventSubscriber* subscriber);
    void hookOnEndSubscriber(SimulationOnEndSubscriber* subscriber);

private:

    boost::tribool evaluateCellOutput(const std::string& cellName, const std::string& output, const std::vector<boost::tribool>& input) const;
    double computeOutputCapacitance(const std::string& outputWire, boost::tribool newState, double defaultOutputCapacitance) const;

    // Observer pattern
    void notifyOnBegin();
    void notifyOnNewEvent(const Event& prevInputEvent, const Event& inputEvent, const Event& outputEvent, const std::string& cellName, const Arc& arc, const std::vector<boost::tribool>& inputStates, double outputCapacitance);
    void notifyAfterHandlingEvent(const Event& ev, unsigned long prevEvTime);
    void notifyOnEnd();

};