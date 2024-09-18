#pragma once

#include "SimulationSubscriber.hpp"
#include "Simulation.hpp"
#include <boost/logic/tribool.hpp>
#include <ostream>
#include <filesystem>
#include <unordered_map>
#include <vector>
#include <set>
#include <string>

std::string toString(boost::tribool);

struct TriboolComp {
    bool operator()(boost::tribool tb1, boost::tribool tb2)  const {
        return tb1.value < tb2.value;
    }
};

class VCDBuffer {
private:

    unsigned long tick;
    std::unordered_map<std::string, std::set<boost::tribool, TriboolComp>> buffer;

public:

    VCDBuffer();
    void insert(const Event& ev);
    void clear();
    void printVCD(std::ostream& os, const std::unordered_map<std::string, std::string>& wireIdMap) const;

};

class VCDOutput
: public virtual SimulationOnBeginSubscriber, public virtual SimulationAfterHandlingEventSubscriber, public virtual SimulationOnEndSubscriber {
private:

    bool toFile;
    std::filesystem::path outputFileName;
    std::ostream& outputFileStream;
    std::ostream& os;
    std::unordered_map<std::string, std::string> wireIdMap;
    VCDBuffer sameTickEvs;

    void initWireIdMap(const std::vector<std::string>& wires);

public:

    VCDOutput() = delete;
    VCDOutput(const std::filesystem::path& outputFileName, std::ostream& outputFileStream, std::ostream& os, const std::vector<std::string>& wires);
    VCDOutput(std::ostream& outputFileStream, std::ostream& os, const std::vector<std::string>& wires);
    void updateOnBegin(Simulation* sim) override;
    void updateAfterHandlingEvent(Simulation* sim, const Event& ev, unsigned long prevEvTime) override;
    void updateOnEnd(Simulation* sim) override;

};