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
            throw std::invalid_argument("invalid stimuli syntax: missing character '='");
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
                throw std::invalid_argument("invalid stimuli syntax: illegal character '" + std::string{c} + "'");
            }
            inputStim.push_back(c == '0' ? false : true);
        }

        std::string pinName = line.substr(0, pos);
        boost::trim(pinName);
        stimuli[pinName] = inputStim;
    }

    return stimuli;
}

bool validStimuli(std::unordered_map<std::string, std::vector<boost::tribool>> stimuli, const VerilogParser& parser) {
    std::set<std::string> moduleIn(parser.module.inputs.begin(), parser.module.inputs.end());
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
    po::options_description desc("Arguments");
    desc.add_options()
        ("help,h",                                                              "Show help message")
        ("limit,l",     po::value<unsigned long>()->default_value(ULONG_MAX),   "Simulation time limit")
        ("verilog,v",   po::value<std::string>()->required(),                   "Verilog source file")
        ("stimuli,s",   po::value<std::string>()->required(),                   "Input vector file")
        ("output,o",    po::value<std::string>(),                               "Output VCD file");

    po::positional_options_description posDesc;
    posDesc.add("verilog", 1);

    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc).positional(posDesc).run(), vm);

    if (vm.count("help")) {
        std::cerr << desc << "\n";
        return EXIT_FAILURE;
    }

    po::notify(vm);

    std::string verilogFile = vm["verilog"].as<std::string>();
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
    parser.read(verilogFile);

    // retrieve stimuli
    std::unordered_map<std::string, std::vector<boost::tribool>> stimuli;
    try {
        stimuli = parseStimuliFile(inputVecFile);
    }
    catch (std::invalid_argument& e) {
        std::cerr << "[ ERRROR ]" << e.what() << "\n";
        return EXIT_FAILURE;
    }

    // validate stimuli
    if (!validStimuli(stimuli, parser)) {
        std::cerr << "[ ERROR ] Stimuli pin list does not match module pin list.\n";
        return EXIT_FAILURE;
    }

    // run simulation
    unsigned long timeLimit = vm["limit"].as<unsigned long>();
    Simulator sim(parser, *os);
    sim.simulate(stimuli, timeLimit);

    if (vm.count("output")) {
        fileOut.close();
    }

    return EXIT_SUCCESS;
}