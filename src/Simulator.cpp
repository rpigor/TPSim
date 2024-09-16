#include "Simulator.hpp"
#include "EventQueue.hpp"
#include "PowerReport.hpp"
#include "Estimator.hpp"
#include "VCD.hpp"
#include "Units.hpp"
#include <iostream>
#include <algorithm>
#include <chrono>

using namespace boost;

Simulator::Simulator(const Module& module, const CellLibrary& lib)
: module(module), lib(lib) {
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

void Simulator::printSimulationHeader(std::ostream& os, const SimulationConfig& cfg) const {
    os << "[ INFO ] Initializing simulation of module \'" << module.name << "\' using library \'" << lib.name << "\'." << std::endl;
    os << "[ INFO ] Input stimuli slope: " << cfg.stimuliSlope << " " << lib.timeUnit << std::endl;
    os << "[ INFO ] Output load capacitance: " << cfg.outputCapacitance << " " << lib.capacitanceUnit << std::endl;
}

void Simulator::printSimulationProgress(std::ostream& os, unsigned long tick, const std::string& tickUnit, char animationChar) const {
    os << "\t\t\t\t\r[ " << animationChar << " ] Simulation time: " << tick << " " << tickUnit << std::flush;
}

void Simulator::printSimulationFooter(std::ostream& os, const std::chrono::steady_clock::time_point& startTime) const {
    os  << "[ INFO ] Finished simulating the module." << std::endl;
    os  << "[ INFO ] It took "
        << std::fixed << std::setprecision(3) << std::chrono::duration<double>(std::chrono::steady_clock::now() - startTime).count()
        << " seconds." << std::endl;
}

boost::tribool Simulator::evaluateCellOutput(const std::string& cellName, const std::string& output, const std::vector<boost::tribool>& input) const {
    Cell cell = lib.cells.at(cellName);
    auto outIt = std::find(cell.outputs.begin(), cell.outputs.end(), output);
    unsigned int outIdx = std::distance(cell.outputs.begin(), outIt);
    return BooleanFunctionVisitor().evaluateExpression(cellOutputExpressions.at(cellName).at(outIdx), cell.inputs, input);
}

double Simulator::getInputStateLeakagePower(const std::string& cellName, const std::vector<boost::tribool>& inputState) const {
    BooleanParser<std::string::iterator> boolParser;
    for (const auto& leakTuple : lib.cells.at(cellName).leakage) {
        auto expr = BooleanFunctionVisitor::parseExpression(leakTuple.first, boolParser);
        if (BooleanFunctionVisitor().evaluateExpression(expr, lib.cells.at(cellName).inputs, inputState)) {
            return leakTuple.second;
        }
    }
    return 0.0;
}

double Simulator::computeOutputCapacitance(const std::string& outputWire, boost::tribool newState, double defaultOutputCapacitance) const {
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

void Simulator::simulate(const std::unordered_map<std::string, std::vector<boost::tribool>>& stimuli, const SimulationConfig& cfg, std::ostream& os) {
    PowerReport pwrReport(cfg.powerReportFile);

    std::vector<std::string> wires;
    for (auto& t : wireStates) {
        wires.push_back(t.first);
    }
    VCDFormatter vcd(os, wires);

    std::size_t animIdx = 0;
    std::array<char, 4> animChars = {'|', '/', '-', '\\'};
    printSimulationHeader(std::cout, cfg);
    printSimulationProgress(std::cout, 0, cfg.timescale, animChars[animIdx]);

    vcd.printHeader(cfg.timescale);
    vcd.printDefinitions(module.name);
    vcd.printVarDumpInit();

    // initiailize event queue with external stimuli
    EventQueue events(stimuli, module.inputs, cfg.clockPeriod, cfg.stimuliSlope);

    // simulation loop
    std::chrono::steady_clock::time_point startTime = std::chrono::steady_clock::now();
    std::unordered_map<std::string, Event> prevGateEvents;
    unsigned long prevTime = events.top().tick;
    // double prevSlope = events.top().inputSlope;
    VCDBuffer sameTickEvs;
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
            events.push(Event{outputWire, inputSlope, result, resultingTick});

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

        if (ev.tick != prevTime) {
            vcd.printVarDumpBuffer(sameTickEvs);
            printSimulationProgress(std::cout, ev.tick, cfg.timescale, animChars[animIdx++]);
            animIdx = animIdx % 4;
        }

        sameTickEvs.insert(ev);
        prevTime = ev.tick;
    }

    vcd.printVarDumpBuffer(sameTickEvs);

    std::cout << std::endl;
    printSimulationFooter(std::cout, startTime);

    pwrReport.saveReport(module.name);
    std::cout << "[ INFO ] Saved power report to " << std::filesystem::canonical(cfg.powerReportFile) << "." << std::endl;
}