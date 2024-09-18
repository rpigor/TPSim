#include "OptionsManager.hpp"
#include "LibertyParser.hpp"
#include "VerilogParser.hpp"
#include "StimuliParser.hpp"
#include "Simulation.hpp"
#include "SimulationOutput.hpp"
#include "VCDOutput.hpp"
#include <boost/iostreams/stream.hpp>
#include <iostream>
#include <string>

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

    // parse Liberty
    std::string libFile = opt.getCellLibraryPath();
    LibertyParser libertyParser;
    try {
        libertyParser.read(libFile);
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERROR ] While parsing \'" << libFile << "\': " << e.what() << "\n";
        return EXIT_FAILURE;
    }
    CellLibrary cellLib = libertyParser.getLibrary();

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
        opt.isExtrapolationEnabled(),
        opt.getPowerReportFilePath()
    };
    Simulation sim(verilogParser.module, cellLib, stimuliParser.getStimuli(), cfg);

    // set simulation output
    SimulationOutput* simOut = new SimulationOutput(std::cout);
    sim.hookOnBeginSubscriber(simOut);
    sim.hookAfterHandlingEventSubscriber(simOut);
    sim.hookOnEndSubscriber(simOut);

    // set VCD output
    std::vector<std::string> wires;
    wires.insert(wires.end(), verilogParser.module.inputs.begin(), verilogParser.module.inputs.end());
    wires.insert(wires.end(), verilogParser.module.outputs.begin(), verilogParser.module.outputs.end());
    wires.insert(wires.end(), verilogParser.module.wires.begin(), verilogParser.module.wires.end());
    boost::iostreams::stream <boost::iostreams::null_sink> nullOs((boost::iostreams::null_sink()));
    std::ostream* vcdFileStream = &nullOs;
    std::ofstream vcdFileOut;
    VCDOutput* vcdOut;
    if (opt.isVCDOutputFileSet()) {
        vcdFileOut.open(opt.getVCDOutputPath());
        vcdFileStream = &vcdFileOut;
        vcdOut = new VCDOutput(opt.getVCDOutputPath(), *vcdFileStream, std::cout, wires);
    }
    else {
        vcdOut = new VCDOutput(*vcdFileStream, std::cout, wires);
    }
    sim.hookOnBeginSubscriber(vcdOut);
    sim.hookAfterHandlingEventSubscriber(vcdOut);
    sim.hookOnEndSubscriber(vcdOut);

    // simulate
    try {
        sim.run();
    }
    catch (std::runtime_error& e) {
        std::cerr << "[ ERRROR ] While simulating: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    delete simOut;
    delete vcdOut;

    // close output file
    if (opt.isVCDOutputFileSet()) {
        vcdFileOut.close();
    }

    return EXIT_SUCCESS;
}