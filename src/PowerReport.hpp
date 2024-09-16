#pragma once

#include <ostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <filesystem>

class PowerReport {
private:

    std::filesystem::path reportPath;
    std::unordered_map<std::string, double> cellSwitchingEnergy;
    std::unordered_map<std::string, double> cellInternalEnergy;
    std::unordered_map<std::string, double> cellLeakageEnergy;

public:

    PowerReport(const std::filesystem::path& reportPath);
    void addSwitchingEnergy(const std::string& cellName, double e);
    void addInternalEnergy(const std::string& cellName, double e);
    void addLeakageEnergy(const std::string& cellName, double e);
    void saveReport(const std::string& moduleName) const;

};