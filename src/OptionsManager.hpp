#pragma once

#include <boost/program_options.hpp>
#include <filesystem>
#include <string>
#include <iostream>

class InvalidOptionException : public std::runtime_error {
public:

   InvalidOptionException(const std::string& option, const std::string& value)
   : runtime_error("\'" + value + "\' is not a valid argument for option \'" + option + "\'") {

   }

};

class FileNotFoundException : public std::runtime_error {
public:

   FileNotFoundException(const std::string& option, const std::string& file)
   : runtime_error("could not find file \'" + file + "\' for option \'" + option + "\'") {

   }

};

class OptionsManager {
private:

    bool help;
    std::filesystem::path cellLibraryPath;
    std::filesystem::path netlistPath;
    std::filesystem::path stimuliPath;
    std::filesystem::path VCDOutputPath;
    bool VCDOutputFileSet;
    std::filesystem::path powerReportFilePath;
    std::string timescale;
    unsigned long period;
    double stimuliSlope;
    double outputCapacitance;
    unsigned long timeout;
    bool extrapolationEnabled;

    boost::program_options::options_description desc;
    std::string programName;

public:

    OptionsManager();
    void parseCLI(int argc, const char* argv[]);
    void printUsage(std::ostream& os) const;

    bool isHelp() const;
    const std::filesystem::path& getCellLibraryPath() const;
    const std::filesystem::path& getNetlistPath() const;
    const std::filesystem::path& getStimuliPath() const;
    const std::filesystem::path& getVCDOutputPath() const;
    bool isVCDOutputFileSet() const;
    const std::filesystem::path& getPowerReportFilePath() const;
    const std::string& getTimescale() const;
    unsigned long getPeriod() const;
    double getStimuliSlope() const;
    double getOutputCapacitance() const;
    unsigned long getTimeout() const;
    bool isExtrapolationEnabled() const;

};