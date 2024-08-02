#include "Simulator.hpp"
#include "CellLibrary.hpp"
#include <boost/program_options.hpp>
#include <boost/logic/tribool.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>
#include <filesystem>

// externally defined standard cell library; see CellLibrary.hpp
extern std::unordered_map<std::string, Cell> cellLib;

namespace po = boost::program_options;

std::unordered_map<std::string, std::vector<boost::tribool>> parseStimuliFile(const std::string& stimuliFile) {
    std::unordered_map<std::string, std::vector<boost::tribool>> stimuli;

    std::ifstream file(stimuliFile);
    std::string line;
    while (std::getline(file, line)) {
        std::size_t pos = line.find('=');
        if (pos == std::string::npos) {
            throw std::runtime_error("missing character '='");
        }

        std::vector<boost::tribool> inputStim;
        std::stringstream isLine(line.substr(pos+1));
        char c;
        while (isLine >> c) {
            if (c == ' ') {
                continue;
            }
            if (c == ';') {
                break;
            }
            if (c != '0' && c != '1') {
                throw std::runtime_error("illegal character '" + std::string{c} + "'");
            }
            inputStim.push_back(c == '0' ? false : true);
        }

        std::string pinName = line.substr(0, pos);
        boost::trim(pinName);
        stimuli[pinName] = inputStim;
    }

    return stimuli;
}

bool validStimuli(std::unordered_map<std::string, std::vector<boost::tribool>> stimuli, const Module& module) {
    std::set<std::string> moduleIn(module.inputs.begin(), module.inputs.end());
    std::set<std::string> inputs;
    for (auto& t : stimuli) {
        inputs.insert(t.first);
    }

    if (moduleIn != inputs) {
        return false;
    }
    return true;
}

void printUsage(std::ostream& os, const po::options_description& description) {
    os  << "Usage: ./tpsim <netlist file> -s <stimuli file> [options...]\n"
        << description << "\n"
        << "Example: ./tpsim module.v -s inputs.stim -o module.vcd --timescale fs --period 100000\n";
}

int main(const int argc, const char* argv[]) {
    po::options_description desc("Options");
    desc.add_options()
        ("help,h",                                                                          "Show help message")
        ("netlist,n",               po::value<std::string>()->required(),                   "Verilog netlist file")
        ("stimuli,s",               po::value<std::string>()->required(),                   "Input vector file")
        ("output,o",                po::value<std::string>(),                               "Output VCD file")
        ("timescale",               po::value<std::string>()->default_value("ps"),          "Simulation timescale")
        ("period",                  po::value<unsigned long>()->default_value(10000),       "Input vector clock period")
        ("time-limit",              po::value<unsigned long>()->default_value(ULONG_MAX),   "Simulation time limit")
        ("disable-extrapolation",                                                           "Disable LUT extrapolation");

    po::positional_options_description posDesc;
    posDesc.add("netlist", 1);

    po::variables_map vm;
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(posDesc).run(), vm);
    }
    catch (po::error& e) {
        std::cerr   << "[ ERRROR ] Incorrect usage: " << e.what() << "\n";
        printUsage(std::cerr, desc);
        return EXIT_FAILURE;
    }

    if (vm.count("help") || argc <= 1) {
        printUsage(std::cerr, desc);
        return EXIT_FAILURE;
    }

    try {
        po::notify(vm);
    }
    catch (po::error& e) {
        std::cerr   << "[ ERRROR ] Incorrect usage: " << e.what() << "\n";
        printUsage(std::cerr, desc);
        return EXIT_FAILURE;
    }

    std::string verilogFile = vm["netlist"].as<std::string>();
    if (!std::filesystem::exists(verilogFile)) {
        std::cerr << "[ ERROR ] File \'" << verilogFile << "\' not found.\n";
        return EXIT_FAILURE;
    }

    std::string inputVecFile = vm["stimuli"].as<std::string>();
    if (!std::filesystem::exists(inputVecFile)) {
        std::cerr << "[ ERROR ] File \'" << inputVecFile << "\' not found.\n";
        return EXIT_FAILURE;
    }

    std::ostream* os = &std::cout;
    std::ofstream fileOut;
    if (vm.count("output")) {
        std::string outputFile = vm["output"].as<std::string>();
        fileOut.open(outputFile);
        os = &fileOut;
    }

    // parse verilog file
    VerilogParser parser(cellLib);
    try {
        parser.read(verilogFile);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERROR ] When parsing \'" << verilogFile << "\':\n" << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // retrieve stimuli
    std::unordered_map<std::string, std::vector<boost::tribool>> stimuli;
    try {
        stimuli = parseStimuliFile(inputVecFile);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERRROR ] When parsing \'" << inputVecFile << "\':\n" << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // validate stimuli
    if (!validStimuli(stimuli, parser.module)) {
        std::cerr << "[ ERROR ] Stimuli pin list does not match module pin list.\n";
        return EXIT_FAILURE;
    }

    // setup simulation
    SimulationConfig cfg{
        vm["timescale"].as<std::string>(),
        vm["period"].as<unsigned long>(),
        vm["time-limit"].as<unsigned long>(),
        vm.count("disable-extrapolation") == 0
    };

    // simulate
    Simulator sim(parser.module, cellLib);
    try {
        sim.simulate(stimuli, cfg, *os);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERRROR ] When simulating: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    if (vm.count("output")) {
        fileOut.close();
    }

    return EXIT_SUCCESS;
}