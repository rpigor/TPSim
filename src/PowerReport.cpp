#include "PowerReport.hpp"
#include <fstream>
#include <filesystem>
#include <iomanip>

PowerReport::PowerReport(const std::filesystem::path& reportPath)
: reportPath(reportPath) {

}

void PowerReport::addSwitchingEnergy(const std::string& cellName, double e) {
    cellSwitchingEnergy[cellName] += e;
}

void PowerReport::addInternalEnergy(const std::string& cellName, double e) {
    cellInternalEnergy[cellName] += e;
}

void PowerReport::addLeakageEnergy(const std::string& cellName, double e) {
    cellLeakageEnergy[cellName] += e;

}

void PowerReport::saveReport(const std::string& moduleName) const {
    const int pctPrecision = 4;
    std::size_t maxCellLen = moduleName.size();

    double circuitSwitchingEnergy = 0.0;
    for (const auto& t : cellSwitchingEnergy) {
        if (maxCellLen < t.first.size()) {
            maxCellLen = t.first.size();
        }
        circuitSwitchingEnergy += t.second;
    }
    double circuitInternalEnergy = 0.0;
    for (const auto& t : cellInternalEnergy) {
        circuitInternalEnergy += t.second;
    }
    double circuitLeakageEnergy = 0.0;
    for (const auto& t : cellLeakageEnergy) {
        circuitLeakageEnergy += t.second;
    }
    double circuitTotalEnergy = circuitSwitchingEnergy + circuitInternalEnergy + circuitLeakageEnergy;

    std::string totalStr = "total";
    std::string switchStr = "switching";
    std::string intStr = "internal";
    std::string leakStr = "leakage";

    std::ofstream report(reportPath);

    report  << std::left << std::setw(moduleName.size()+2) << std::setfill(' ') << ""
            << std::right << std::setw(totalStr.size()+8) << std::setfill(' ') << totalStr
            << std::right << std::setw(switchStr.size()+8) << std::setfill(' ') << switchStr
            << std::right << std::setw(intStr.size()+8) << std::setfill(' ') << intStr
            << std::right << std::setw(leakStr.size()+8) << std::setfill(' ') << leakStr
            << std::endl;

    report  << std::setw(moduleName.size()+totalStr.size()+switchStr.size()+intStr.size()+leakStr.size()+35) << std::setfill('-') << "\n";

    report  << std::left << std::setw(moduleName.size()+2) << std::setfill(' ') << moduleName
            << std::right << std::setprecision(totalStr.size()) << std::setw(totalStr.size()+8) << std::setfill(' ') << circuitTotalEnergy
            << std::right << std::setprecision(switchStr.size()) << std::setw(switchStr.size()+8) << std::setfill(' ') << circuitSwitchingEnergy
            << std::right << std::setprecision(intStr.size()) << std::setw(intStr.size()+8) << std::setfill(' ') << circuitInternalEnergy
            << std::right << std::setprecision(leakStr.size()) << std::setw(leakStr.size()+8) << std::setfill(' ') << circuitLeakageEnergy
            << std::endl;

    std::stringstream switchPct;
    switchPct << std::setprecision(pctPrecision) << 100*(circuitSwitchingEnergy/circuitTotalEnergy) << "%";
    std::stringstream intPct;
    intPct << std::setprecision(pctPrecision) << 100*(circuitInternalEnergy/circuitTotalEnergy) << "%";
    std::stringstream leakPct;
    leakPct << std::setprecision(pctPrecision) << 100*(circuitLeakageEnergy/circuitTotalEnergy) << "%";

    report  << std::left << std::setw(moduleName.size()+2) << std::setfill(' ') << ""
            << std::right << std::setprecision(pctPrecision) << std::setw(totalStr.size()+8) << std::setfill(' ') << "100.00%"
            << std::right << std::setprecision(pctPrecision) << std::setw(switchStr.size()+8) << std::setfill(' ') << switchPct.str()
            << std::right << std::setprecision(pctPrecision) << std::setw(intStr.size()+8) << std::setfill(' ') << intPct.str()
            << std::right << std::setprecision(pctPrecision) << std::setw(leakStr.size()+8) << std::setfill(' ') << leakPct.str()
            << std::endl;

    report.close();
}