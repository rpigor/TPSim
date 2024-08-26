#include "Simulator.hpp"
#include "OptionsManager.hpp"
#include "VerilogParser.hpp"
#include "StimuliParser.hpp"
#include "CellLibrary.hpp"
#include <string>
#include <vector>
#include <unordered_map>

// externally defined standard cell library; see CellLibrary.cpp
extern CellLibrary cellLib;

int main(const int argc, const char* argv[]) {

    // parse command-line arguments
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

    // set output stream
    std::ostream* os = &std::cout;
    std::ofstream fileOut;
    if (opt.isOutputFileSet()) {
        fileOut.open(opt.getOutputPath());
        os = &fileOut;
    }

    // parse Verilog
    std::string netlistFile = opt.getNetlistPath();
    VerilogParser verilogParser(cellLib.cells);
    try {
        verilogParser.read(netlistFile);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERROR ] While parsing \'" << netlistFile << "\': " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // parse stimuli
    std::string inputVecFile = opt.getStimuliPath();
    StimuliParser stimuliParser(verilogParser.module.inputs);
    try {
        stimuliParser.read(inputVecFile);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERRROR ] While parsing \'" << inputVecFile << "\': " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // set up simulation
    SimulationConfig cfg{
        opt.getTimescale(),
        opt.getPeriod(),
        opt.getStimuliSlope(),
        opt.getOutputCapacitance(),
        opt.getTimeout(),
        opt.isExtrapolationEnabled()
    };

    // simulate
    Simulator sim(verilogParser.module, cellLib);
    try {
        sim.simulate(stimuliParser.getStimuli(), cfg, *os);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERRROR ] While simulating: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // close output file
    if (opt.isOutputFileSet()) {
        fileOut.close();
    }

    return EXIT_SUCCESS;
}