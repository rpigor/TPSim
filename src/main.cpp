#include "Simulator.hpp"
#include "OptionsManager.hpp"
#include "CellLibrary.hpp"
#include <boost/logic/tribool.hpp>
#include <boost/algorithm/string.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <set>

// externally defined standard cell library; see CellLibrary.hpp
extern std::unordered_map<std::string, Cell> cellLib;

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

int main(const int argc, const char* argv[]) {
    OptionsManager opt;
    try {
        opt.parseCLI(argc, argv);
    }
    catch (const std::runtime_error& e) {
        std::cerr << "[ ERROR ] While parsing command-line arguments: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    if (opt.isHelp()) {
        opt.printUsage(std::cout);
        return EXIT_FAILURE;
    }

    std::ostream* os = &std::cout;
    std::ofstream fileOut;
    if (opt.isOutputFileSet()) {
        fileOut.open(opt.getOutputPath());
        os = &fileOut;
    }

    // parse Verilog
    std::string netlistFile = opt.getNetlistPath();
    VerilogParser parser(cellLib);
    try {
        parser.read(netlistFile);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERROR ] While parsing \'" << netlistFile << "\': " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // retrieve stimuli
    std::string inputVecFile = opt.getStimuliPath();
    std::unordered_map<std::string, std::vector<boost::tribool>> stimuli;
    try {
        stimuli = parseStimuliFile(inputVecFile);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERRROR ] While parsing \'" << inputVecFile << "\': " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // validate stimuli
    if (!validStimuli(stimuli, parser.module)) {
        std::cerr << "[ ERROR ] Stimuli pin list does not match module pin list\n";
        return EXIT_FAILURE;
    }

    // set up simulation
    SimulationConfig cfg{
        opt.getTimescale(),
        opt.getPeriod(),
        opt.getTimeout(),
        opt.isExtrapolationEnabled()
    };

    // simulate
    Simulator sim(parser.module, cellLib);
    try {
        sim.simulate(stimuli, cfg, *os);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERRROR ] While simulating: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    if (opt.isOutputFileSet()) {
        fileOut.close();
    }

    return EXIT_SUCCESS;
}