#include "Simulation.hpp"
#include "TimingWheel.hpp"
#include "Estimator.hpp"
#include "Units.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

using namespace boost;

Simulation::Simulation(const Module& module, const CellLibrary& lib, const Stimuli& stim, const SimulationConfig& cfg)
: module(module), lib(lib), stim(stim), cfg(cfg) {
    for (auto& w : module.wires) {
        wireStates[w] = indeterminate;
    }

    for (auto& w : module.inputs) {
        wireStates[w] = indeterminate;
    }

    for (auto& w : module.outputs) {
        wireStates[w] = indeterminate;
    }

    // parse cell output boolean expressions
    BooleanParser<std::string::iterator> boolParser;
    for (auto& [cellName, cell] : lib.cells) {
        for (unsigned int outIdx = 0; outIdx < cell.bitFunctions.size(); outIdx++) {
            std::string boolFunc = cell.bitFunctions[outIdx];
            auto expr = BooleanFunctionVisitor::parseExpression(boolFunc, boolParser);
            cellOutputExpressions[cellName].push_back(expr);
        }
    }
}

boost::tribool Simulation::evaluateCellOutput(const std::string& cellName, const std::string& output, const std::vector<boost::tribool>& input) const {
    auto outIt = std::find(lib.cells.at(cellName).outputs.begin(), lib.cells.at(cellName).outputs.end(), output);
    unsigned int outIdx = std::distance(lib.cells.at(cellName).outputs.begin(), outIt);
    return BooleanFunctionVisitor().evaluateExpression(cellOutputExpressions.at(cellName).at(outIdx), lib.cells.at(cellName).inputs, input);
}

double Simulation::computeOutputCapacitance(const std::string& outputWire, boost::tribool newState, double defaultOutputCapacitance) const {
    int affectedGates = 0;
    double outputCap = 0.0;
    for (auto& g : module.gates) {
        auto inputIt = g.net2input.find(outputWire);
        bool affectsGateInput = inputIt != g.net2input.end();
        if (!affectsGateInput) {
            continue;
        }
        outputCap += lib.cells.at(g.cell).pinCapacitance.at(inputIt->second);
        affectedGates++;
    }

    if (affectedGates == 0) { // last stage
        return defaultOutputCapacitance;
    }

    return outputCap;
}

void Simulation::run() {
    notifyOnBegin();

    // initiailize timing wheel with external input stimuli
    TimingWheel wheel(cfg.wheelSize, stim, cfg.stimuliSlope, cfg.clockPeriod); // calculate the critical path delay before defining the timing wheel

    // simulation loop
    std::unordered_map<std::string, Event> prevGateEvents;
    unsigned long prevTime = wheel.getCurrentEventTick();
    auto nextEvs = wheel.consumeNextEvents();
    while (!nextEvs.empty()) {
        for (const auto& ev : nextEvs) {
            if (ev.canceled) { // ignore aborted events
                continue;
            }

            if (ev.tick > cfg.timeLimit) { // ignore events happening after simulation stop time
                continue;
            }

            wireStates[ev.wire] = ev.value; // update circuit state

            // handles gates affected by the event
            for (auto& g : module.gates) {
                auto inputIt = g.net2input.find(ev.wire);
                bool affectsGateInput = inputIt != g.net2input.end();
                if (!affectsGateInput) {
                    continue;
                }

                std::string inputPin = inputIt->second;
                std::vector<tribool> inputStates;
                for (const std::string& in : lib.cells.at(g.cell).inputs) {
                    if (in == inputPin) {
                        inputStates.push_back(ev.value);
                        continue;
                    }
                    std::string inWire = g.input2net.at(in);
                    inputStates.push_back(wireStates.at(inWire));
                }

                Event prevEvent;
                try {
                    prevEvent = prevGateEvents.at(g.name);
                    prevGateEvents[g.name] = ev;
                }
                catch (const std::out_of_range& e) {
                    prevEvent = Event{"", 0.0, boost::indeterminate, 0};
                    prevGateEvents[g.name] = ev;
                }

                // compute new event
                std::string outputPin = lib.cells.at(g.cell).outputs[0]; // only a single output is currently supported!
                std::string outputWire;
                try {
                    outputWire = g.output2net.at(outputPin);
                }
                catch (const std::out_of_range& e) {
                    throw std::runtime_error("could not find output pin \'" + outputPin + "\' in instance \'" + g.name + "\' of type \'" + g.cell + "\'.");
                }
                tribool result = evaluateCellOutput(g.cell, outputPin, inputStates);
                tribool outputState;
                try {
                    outputState = wireStates.at(outputWire);
                }
                catch (const std::out_of_range& e) {
                    throw std::runtime_error("could not find wire \'" + outputWire + "\' (used in instance \'" + g.name + "\' of type \'" + g.cell + "\').");
                }
                if ((outputState == result) || (indeterminate(outputState) && indeterminate(result))) {
                    continue;
                }

                // estimate Event parameters
                double outputCap = computeOutputCapacitance(outputWire, result, cfg.outputCapacitance);
                Arc arc{inputPin, outputPin};
                double delay = indeterminate(result) ? 0.0 : Estimator::estimate(lib.cells.at(g.cell).delay, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
                double inputSlope = indeterminate(result) ? 0.0 : Estimator::estimate(lib.cells.at(g.cell).outputSlope, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
                unsigned long resultingTick = ev.tick + Units::timeToTick(delay, lib.timeUnit, cfg.timescale);
                Event newEv{outputWire, inputSlope, result, resultingTick};
                wheel.scheduleEvent(newEv);

                notifyOnNewEvent(prevEvent, ev, newEv, g.cell, arc, inputStates, outputCap);
            }

            notifyAfterHandlingEvent(ev, prevTime);

            prevTime = ev.tick;
        }

        nextEvs = wheel.consumeNextEvents();
    }

    notifyOnEnd();
}

const Module& Simulation::getModule() const {
    return module;
}

const CellLibrary& Simulation::getLibrary() const {
    return lib;
}

Stimuli Simulation::getStimuli() const {
    return stim;
}

SimulationConfig Simulation::getConfiguration() const {
    return cfg;
}

void Simulation::hookOnBeginSubscriber(SimulationOnBeginSubscriber* subscriber) {
    beginSubscribers.push_back(subscriber);
}

void Simulation::hookOnNewEventSubscriber(SimulationOnNewEventSubscriber* subscriber) {
    newEventSubscribers.push_back(subscriber);
}

void Simulation::hookAfterHandlingEventSubscriber(SimulationAfterHandlingEventSubscriber* subscriber) {
    afterEventSubscribers.push_back(subscriber);
}

void Simulation::hookOnEndSubscriber(SimulationOnEndSubscriber* subscriber) {
    endSubscribers.push_back(subscriber);
}

void Simulation::notifyOnBegin() {
    for (auto s : beginSubscribers) {
        s->updateOnBegin(this);
    }
}

void Simulation::notifyOnNewEvent(const Event& prevInputEvent, const Event& inputEvent, const Event& outputEvent, const std::string& cellName, const Arc& arc, const std::vector<boost::tribool>& inputStates, double outputCapacitance) {
    for (auto s : newEventSubscribers) {
        s->updateOnNewEvent(this, prevInputEvent, inputEvent,outputEvent, cellName, arc, inputStates, outputCapacitance);
    }
}

void Simulation::notifyAfterHandlingEvent(const Event& ev, unsigned long prevEvTime) {
    for (auto s : afterEventSubscribers) {
        s->updateAfterHandlingEvent(this, ev, prevEvTime);
    }
}

void Simulation::notifyOnEnd() {
    for (auto s : endSubscribers) {
        s->updateOnEnd(this);
    }
}