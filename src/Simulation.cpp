#include "Simulation.hpp"
#include "EventQueue.hpp"
#include "PowerReport.hpp"
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
    for (auto& t : lib.cells) {
        std::string cellName = t.first;
        Cell cell = t.second;
        for (unsigned int outIdx = 0; outIdx < cell.bitFunctions.size(); outIdx++) {
            std::string boolFunc = cell.bitFunctions[outIdx];
            auto expr = BooleanFunctionVisitor::parseExpression(boolFunc, boolParser);
            cellOutputExpressions[cellName].push_back(expr);
        }
    }
}

boost::tribool Simulation::evaluateCellOutput(const std::string& cellName, const std::string& output, const std::vector<boost::tribool>& input) const {
    Cell cell = lib.cells.at(cellName);
    auto outIt = std::find(cell.outputs.begin(), cell.outputs.end(), output);
    unsigned int outIdx = std::distance(cell.outputs.begin(), outIt);
    return BooleanFunctionVisitor().evaluateExpression(cellOutputExpressions.at(cellName).at(outIdx), cell.inputs, input);
}

double Simulation::getInputStateLeakagePower(const std::string& cellName, const std::vector<boost::tribool>& inputState) const {
    BooleanParser<std::string::iterator> boolParser;
    for (const auto& leakTuple : lib.cells.at(cellName).leakage) {
        auto expr = BooleanFunctionVisitor::parseExpression(leakTuple.first, boolParser);
        if (BooleanFunctionVisitor().evaluateExpression(expr, lib.cells.at(cellName).inputs, inputState)) {
            return leakTuple.second;
        }
    }
    return 0.0;
}

double Simulation::computeOutputCapacitance(const std::string& outputWire, boost::tribool newState, double defaultOutputCapacitance) const {
    int affectedGates = 0;
    double outputCap = 0.0;
    for (auto& g : module.gates) {
        Cell cell = lib.cells.at(g.cell);

        auto inputIt = g.net2input.find(outputWire);
        bool affectsGateInput = inputIt != g.net2input.end();
        if (!affectsGateInput) {
            continue;
        }
        outputCap += cell.pinCapacitance.at(inputIt->second);
        affectedGates++;
    }

    if (affectedGates == 0) { // last stage
        return defaultOutputCapacitance;
    }

    return outputCap;
}

void Simulation::run() {
    PowerReport pwrReport(cfg.powerReportFile);

    notifyOnBegin();

    // initiailize event queue with external stimuli
    EventQueue events(stim, module.inputs, cfg.clockPeriod, cfg.stimuliSlope);

    // simulation loop
    std::unordered_map<std::string, Event> prevGateEvents;
    unsigned long prevTime = events.top().tick;
    // double prevSlope = events.top().inputSlope;

    while (!events.empty()) {
        Event ev = events.top();
        events.pop();

        if (ev.tick > cfg.timeLimit) { // ignore events happening after simulation stop time
            continue;
        }

        wireStates[ev.wire] = ev.value;

        // handles gates affected by the event
        for (auto& g : module.gates) {
            Cell cell = lib.cells.at(g.cell);

            auto inputIt = g.net2input.find(ev.wire);
            bool affectsGateInput = inputIt != g.net2input.end();
            if (!affectsGateInput) {
                continue;
            }

            std::string inputPin = inputIt->second;
            std::vector<tribool> inputStates;
            for (const std::string& in : cell.inputs) {
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
            std::string outputPin = cell.outputs[0]; // only a single output is supported!
            std::string outputWire;
            try {
                outputWire = g.output2net.at(outputPin);
            }
            catch (const std::out_of_range& e) {
                throw std::runtime_error("could not find output pin \'" + outputPin + "\' in instance \'" + g.name + "\' of type \'" + g.cell + "\'.");
            }
            tribool result = evaluateCellOutput(cell.name, outputPin, inputStates);
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
            double delay = indeterminate(result) ? 0.0 : Estimator::estimate(lib.cells.at(cell.name).delay, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
            double inputSlope = indeterminate(result) ? 0.0 : Estimator::estimate(lib.cells.at(cell.name).outputSlope, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
            unsigned long resultingTick = ev.tick + Units::timeToTick(delay, lib.timeUnit, cfg.timescale);
            Event newEv{outputWire, inputSlope, result, resultingTick};
            events.push(newEv);

            notifyOnNewEvent(newEv, ev, cell.name, outputCap);

            if (ev.tick < cfg.clockPeriod) { // ignore first transitions, when the circuit is in an indeterminate state
                continue;
            }

            // estimate dynamic energy
            double energyScale = Units::unitScale(lib.capacitanceUnit)*Units::unitScale(lib.voltageUnit)*Units::unitScale(lib.voltageUnit);
            double internalEnergy = indeterminate(result) ? 0.0 : Estimator::estimate(lib.cells.at(cell.name).internalPower, arc, ev.inputSlope, outputCap, result ? true : false, cfg.allowExtrapolation);
            double internalEnergyScaled = internalEnergy*energyScale;
            double switchingEnergy = outputCap*lib.voltage*lib.voltage / 2;
            double switchingEnergyScaled = switchingEnergy*energyScale;
            // unsigned long eventEndTick = Estimator::estimateEndTime(ev.tick, ev.inputSlope, lib.timeUnit, cfg.timescale);
            pwrReport.addSwitchingEnergy(g.cell, switchingEnergyScaled);
            pwrReport.addInternalEnergy(g.cell, internalEnergyScaled);

            // estimate leakage energy
            if (ev.tick != 0) { // ignore first event
                double leakageEnergy;
                double leakagePower = getInputStateLeakagePower(cell.name, inputStates);
                unsigned long startLeakTick = Estimator::estimateEndTime(prevEvent.tick, prevEvent.inputSlope, lib.timeUnit, cfg.timescale);
                long leakageInterval = ev.tick - startLeakTick; // from the end of the previous event to the start of the current event
                if (leakageInterval < 0) {
                    startLeakTick = prevEvent.tick;
                    leakageEnergy = leakagePower*Units::tickToTime(ev.tick - prevEvent.tick, lib.timeUnit, cfg.timescale)*(Units::unitScale(lib.timeUnit)*Units::unitScale(lib.leakagePowerUnit));
                }
                else {
                    leakageEnergy = leakagePower*Units::tickToTime(leakageInterval, lib.timeUnit, cfg.timescale)*(Units::unitScale(lib.timeUnit)*Units::unitScale(lib.leakagePowerUnit));
                }
                pwrReport.addLeakageEnergy(g.cell, leakageEnergy);
            }
        }

        notifyAfterHandlingEvent(ev, prevTime);

        prevTime = ev.tick;
    }

    notifyOnEnd();

    pwrReport.saveReport(module.name);
    std::cout << "[ INFO ] Saved power report to " << std::filesystem::canonical(cfg.powerReportFile) << "." << std::endl;
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

void Simulation::notifyOnNewEvent(const Event& newEv, const Event& causeEv, const std::string& cellName, double outputCapacitance) {
    for (auto s : newEventSubscribers) {
        s->updateOnNewEvent(this, newEv, causeEv, cellName, outputCapacitance);
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