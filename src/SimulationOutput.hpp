#pragma once

#include "SimulationSubscriber.hpp"
#include "Simulation.hpp"
#include <ostream>
#include <array>
#include <chrono>

class SimulationOutput
: public virtual SimulationOnBeginSubscriber, public virtual SimulationAfterHandlingEventSubscriber, public virtual SimulationOnEndSubscriber {
private:

    std::ostream& os;
    std::chrono::steady_clock::time_point simStartTime;

    std::array<char, 4> animationCharacters;
    std::size_t animationIdx;

public:

    SimulationOutput(std::ostream& os);
    void updateOnBegin(Simulation* sim) override;
    void updateAfterHandlingEvent(Simulation* sim, const Event& ev, unsigned long prevEvTime) override;
    void updateOnEnd(Simulation* sim) override;

};