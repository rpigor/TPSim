#include "SimulationOutput.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>

SimulationOutput::SimulationOutput(std::ostream& os)
: os(os), animationCharacters({'|', '/', '-', '\\'}), animationIdx(0) {

}

void SimulationOutput::updateOnBegin(Simulation* sim) {
    const auto& lib = sim->getLibrary();
    auto cfg = sim->getConfiguration();
    os << "[ INFO ] Initializing simulation of module \'" << sim->getModule().name << "\' using library \'" << lib.name << "\'." << std::endl;
    os << "[ INFO ] Input stimuli slope: " << cfg.stimuliSlope << " " << lib.timeUnit << std::endl;
    os << "[ INFO ] Output load capacitance: " << cfg.outputCapacitance << " " << lib.capacitanceUnit << std::endl;
    os << "\t\t\t\t\r[ " << animationCharacters[animationIdx++] << " ] Simulation time: " << 0 << " " << cfg.timescale << std::flush;

    animationIdx = animationIdx % animationCharacters.size();
    simStartTime = std::chrono::steady_clock::now();
}

void SimulationOutput::updateAfterHandlingEvent(Simulation* sim, const Event& ev, unsigned long prevEvTime) {
    os << "\t\t\t\t\r[ " << animationCharacters[animationIdx++] << " ] Simulation time: " << ev.tick << " " << sim->getConfiguration().timescale << std::flush;
    animationIdx = animationIdx % animationCharacters.size();
}

void SimulationOutput::updateOnEnd(Simulation* sim) {
    os  << std::endl;
    os  << "[ INFO ] Finished simulating the module." << std::endl;
    os  << "[ INFO ] The simulation took "
        << std::fixed << std::setprecision(3) << std::chrono::duration<double>(std::chrono::steady_clock::now() - simStartTime).count()
        << " seconds." << std::endl;
}