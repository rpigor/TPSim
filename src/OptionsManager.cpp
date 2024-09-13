#include "OptionsManager.hpp"
#include "Units.hpp"
#include <boost/program_options.hpp>

namespace po = boost::program_options;

OptionsManager::OptionsManager()
: desc("Options"), programName("tpsim") {

}

void OptionsManager::parseCLI(int argc, const char* argv[]) {
    desc.add_options()
        ("help,h",                                                                                  "Show help message")
        ("netlist,n",               po::value<std::string>()->required(),                           "Verilog netlist file")
        ("stimuli,s",               po::value<std::string>()->required(),                           "Input vector file")
        ("library,l",               po::value<std::string>()->required(),                           "Liberty standard cell library")
        ("vcd-output",              po::value<std::string>(),                                       "VCD output file")
        ("power-report",            po::value<std::string>(),                                       "Power report file")
        ("timescale",               po::value<std::string>(&timescale)->default_value("ps"),        "Simulation timescale")
        ("period",                  po::value<unsigned long>(&period)->default_value(10000),        "Input stimuli clock period")
        ("input-slope",             po::value<double>(&stimuliSlope)->default_value(0.0),           "Input stimuli slope")
        ("output-capacitance",      po::value<double>(&outputCapacitance)->default_value(0.0),      "Output capacitance")
        ("timeout",                 po::value<unsigned long>(&timeout)->default_value(ULONG_MAX),   "Simulation time limit")
        ("disable-extrapolation",                                                                   "Disable LUT extrapolation");

    po::positional_options_description posDesc;
    posDesc.add("netlist", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(posDesc).run(), vm);

    help = vm.count("help") != 0;
    extrapolationEnabled = vm.count("disable-extrapolation") == 0;

    if (help || argc <= 1) {
        return;
    }

    po::notify(vm);

    cellLibraryPath = std::filesystem::path(vm["library"].as<std::string>());
    if (!std::filesystem::exists(cellLibraryPath)) {
        throw FileNotFoundException("lilbrary", cellLibraryPath);
    }

    netlistPath = std::filesystem::path(vm["netlist"].as<std::string>());
    if (!std::filesystem::exists(netlistPath)) {
        throw FileNotFoundException("netlist", netlistPath);
    }

    stimuliPath = std::filesystem::path(vm["stimuli"].as<std::string>());
    if (!std::filesystem::exists(stimuliPath)) {
        throw FileNotFoundException("stimuli", stimuliPath);
    }

    VCDOutputFileSet = vm.count("vcd-output") != 0;
    if (VCDOutputFileSet) {
        VCDOutputPath = std::filesystem::path(vm["vcd-output"].as<std::string>());
    }

    if (vm.count("power-report") != 0) {
        powerReportFilePath = std::filesystem::path(vm["power-report"].as<std::string>());
    }
    else {
        std::time_t t = std::time(nullptr);
        std::tm tm = *std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(&tm, "power-%d%m%Y-%H%M%S.rpt");
        powerReportFilePath = ss.str();
    }

    if (stimuliSlope < 0.0) {
        throw InvalidOptionException("input-slope", std::to_string(stimuliSlope));
    }

    if (outputCapacitance < 0.0) {
        throw InvalidOptionException("output-capacitance", std::to_string(outputCapacitance));
    }

    if (!Units::isValidUnit(timescale)) {
        throw InvalidOptionException("timescale", timescale);
    }
}

void OptionsManager::printUsage(std::ostream& os) const {
    os  << "Usage: ./" << programName << " <netlist file> -s <stimuli file> -l <library file> [options...]\n"
        << desc << "\n"
        << "Example: ./" << programName << " module.v -s inputs.stim -l sky130.lib --vcd-output module.vcd --timescale fs --period 100000\n";
}

bool OptionsManager::isHelp() const {
    return help;
}

const std::filesystem::path& OptionsManager::getCellLibraryPath() const {
    return cellLibraryPath;
}

const std::filesystem::path& OptionsManager::getNetlistPath() const {
    return netlistPath;
}

const std::filesystem::path& OptionsManager::getStimuliPath() const {
    return stimuliPath;
}

const std::filesystem::path& OptionsManager::getVCDOutputPath() const {
    return VCDOutputPath;
}

bool OptionsManager::isVCDOutputFileSet() const {
    return VCDOutputFileSet;
}

const std::filesystem::path& OptionsManager::getPowerReportFilePath() const {
    return powerReportFilePath;
}

const std::string& OptionsManager::getTimescale() const {
    return timescale;
}

unsigned long OptionsManager::getPeriod() const {
    return period;
}

double OptionsManager::getStimuliSlope() const {
    return stimuliSlope;
}

double OptionsManager::getOutputCapacitance() const {
    return outputCapacitance;
}

unsigned long OptionsManager::getTimeout() const {
    return timeout;
}

bool OptionsManager::isExtrapolationEnabled() const {
    return extrapolationEnabled;
}