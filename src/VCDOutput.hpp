#pragma once

#include "SimulationSubscriber.hpp"
#include "Simulation.hpp"
#include "VCD.hpp"
#include <ostream>
#include <filesystem>

class VCDOutput
: public virtual SimulationOnBeginSubscriber, public virtual SimulationAfterHandlingEventSubscriber, public virtual SimulationOnEndSubscriber {
private:

    bool toFile;
    std::filesystem::path outputFileName;
    std::ostream& outputFileStream;
    std::ostream& os;
    VCDFormatter vcd;
    VCDBuffer sameTickEvs;

public:

    VCDOutput() = delete;
    VCDOutput(const std::filesystem::path& outputFileName, std::ostream& outputFileStream, std::ostream& os, const std::vector<std::string>& wires);
    VCDOutput(std::ostream& outputFileStream, std::ostream& os, const std::vector<std::string>& wires);
    void updateOnBegin(Simulation* sim) override;
    void updateAfterHandlingEvent(Simulation* sim, const Event& ev, unsigned long prevEvTime) override;
    void updateOnEnd(Simulation* sim) override;

};