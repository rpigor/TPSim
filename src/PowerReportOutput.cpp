#include "PowerReportOutput.hpp"
#include "BooleanParser.hpp"
#include "Estimator.hpp"
#include "Units.hpp"
#include <boost/logic/tribool.hpp>
#include <fstream>
#include <string>
#include <filesystem>
#include <iomanip>

PowerReportOutput::PowerReportOutput(const std::filesystem::path& reportPath, std::ostream& os)
: reportPath(reportPath), os(os) {

}

void PowerReportOutput::saveReport(const std::string& moduleName) const {
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

    os << "[ INFO ] Saved power report to " << std::filesystem::canonical(reportPath) << "." << std::endl;
}

double PowerReportOutput::getInputStateLeakagePower(const CellLibrary& lib, const std::string& cellName, const std::vector<boost::tribool>& inputStates) const {
    BooleanParser<std::string::iterator> boolParser;
    for (const auto& leakTuple : lib.cells.at(cellName).leakage) {
        auto expr = BooleanFunctionVisitor::parseExpression(leakTuple.first, boolParser);
        if (BooleanFunctionVisitor().evaluateExpression(expr, lib.cells.at(cellName).inputs, inputStates)) {
            return leakTuple.second;
        }
    }
    return 0.0;
}

void PowerReportOutput::updateOnNewEvent(Simulation* sim, const Event& prevInputEvent, const Event& inputEvent, const Event& outputEvent, const std::string& cellName, const Arc& arc, const std::vector<boost::tribool>& inputStates, double outputCapacitance) {
    if (inputEvent.tick < sim->getConfiguration().clockPeriod) { // ignore first transitions, when the circuit is in an indeterminate state
        return;
    }

    const auto& lib = sim->getLibrary();
    auto cfg = sim->getConfiguration();

    // estimate dynamic energy
    double energyScale = Units::unitScale(lib.capacitanceUnit)*Units::unitScale(lib.voltageUnit)*Units::unitScale(lib.voltageUnit);
    double internalEnergy = boost::indeterminate(outputEvent.value) ? 0.0 : Estimator::estimate(lib.cells.at(cellName).internalPower, arc, inputEvent.inputSlope, outputCapacitance, outputEvent.value ? true : false, cfg.allowExtrapolation);
    double internalEnergyScaled = internalEnergy*energyScale;
    double switchingEnergy = outputCapacitance*lib.voltage*lib.voltage / 2;
    double switchingEnergyScaled = switchingEnergy*energyScale;
    cellSwitchingEnergy[cellName] += switchingEnergyScaled;
    cellInternalEnergy[cellName] += internalEnergyScaled;

    // estimate leakage energy
    if (inputEvent.tick != 0) { // ignore first event
        double leakageEnergy;
        double leakagePower = getInputStateLeakagePower(lib, cellName, inputStates);
        unsigned long startLeakTick = Estimator::estimateEndTime(prevInputEvent.tick, prevInputEvent.inputSlope, lib.timeUnit, cfg.timescale);
        long leakageInterval = inputEvent.tick - startLeakTick; // from the end of the previous event to the start of the current event
        if (leakageInterval < 0) {
            startLeakTick = prevInputEvent.tick;
            leakageEnergy = leakagePower*Units::tickToTime(inputEvent.tick - prevInputEvent.tick, lib.timeUnit, cfg.timescale)*(Units::unitScale(lib.timeUnit)*Units::unitScale(lib.leakagePowerUnit));
        }
        else {
            leakageEnergy = leakagePower*Units::tickToTime(leakageInterval, lib.timeUnit, cfg.timescale)*(Units::unitScale(lib.timeUnit)*Units::unitScale(lib.leakagePowerUnit));
        }
        cellLeakageEnergy[cellName] += leakageEnergy;
    }
}

void PowerReportOutput::updateOnEnd(Simulation* sim) {
    saveReport(sim->getModule().name);
}