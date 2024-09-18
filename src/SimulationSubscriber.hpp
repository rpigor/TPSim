#pragma once

#include "Simulation.hpp"
#include "Event.hpp"

class Simulation;

class SimulationOnBeginSubscriber {
public:

    virtual ~SimulationOnBeginSubscriber() = default;
    virtual void updateOnBegin(Simulation* sim) = 0;

};

class SimulationOnNewEventSubscriber {
public:

    virtual ~SimulationOnNewEventSubscriber() = default;
    virtual void updateOnNewEvent(Simulation* sim, const Event& prevInputEvent, const Event& inputEvent, const Event& outputEvent, const std::string& cellName, const Arc& arc, const std::vector<boost::tribool>& inputStates, double outputCapacitance) = 0;

};

class SimulationAfterHandlingEventSubscriber {
public:

    virtual ~SimulationAfterHandlingEventSubscriber() = default;
    virtual void updateAfterHandlingEvent(Simulation* sim, const Event& ev, unsigned long prevEvTime) = 0;

};

class SimulationOnEndSubscriber {
public:

    virtual ~SimulationOnEndSubscriber() = default;
    virtual void updateOnEnd(Simulation* sim) = 0;

};