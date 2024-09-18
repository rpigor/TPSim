#pragma once

#include "SimulationSubscriber.hpp"
#include "CellLibrary.hpp"
#include <boost/logic/tribool.hpp>
#include <ostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <filesystem>

class PowerReportOutput
: public virtual SimulationOnNewEventSubscriber, public virtual SimulationOnEndSubscriber {
private:

    std::filesystem::path reportPath;
    std::ostream& os;
    std::unordered_map<std::string, double> cellSwitchingEnergy;
    std::unordered_map<std::string, double> cellInternalEnergy;
    std::unordered_map<std::string, double> cellLeakageEnergy;

public:

    PowerReportOutput(const std::filesystem::path& reportPath, std::ostream& os);
    void updateOnNewEvent(Simulation* sim, const Event& prevInputEvent, const Event& inputEvent, const Event& outputEvent, const std::string& cellName, const Arc& arc, const std::vector<boost::tribool>& inputStates, double outputCapacitance) override;
    void updateOnEnd(Simulation* sim) override;

private:

    void saveReport(const std::string& moduleName) const;
    double getInputStateLeakagePower(const CellLibrary& lib, const std::string& cellName, const std::vector<boost::tribool>& inputState) const;

};