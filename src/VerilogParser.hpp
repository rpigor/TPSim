#pragma once

#include "verilog_driver.hpp"
#include "Cell.hpp"
#include <string>
#include <unordered_map>
#include <vector>

struct Gate {
    std::string name;
    std::string cell;
    std::unordered_map<std::string, std::string> input2net;
    std::unordered_map<std::string, std::string> output2net;
    std::unordered_map<std::string, std::string> net2input;
    std::unordered_map<std::string, std::string> net2output;
};

struct Module {
    std::string name;
    std::vector<std::string> ports;
    std::vector<std::string> wires;
    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    std::vector<Gate> gates;
};

class VerilogParser : public verilog::ParserVerilogInterface {
public:

    const std::unordered_map<std::string, Cell>& lib;
    Module module;

    VerilogParser() = delete;
    VerilogParser(const std::unordered_map<std::string, Cell>& lib) : lib(lib) { }
    virtual ~VerilogParser() { }

    void add_module(std::string&& name);
    void add_port(verilog::Port&& port);
    void add_assignment(verilog::Assignment&& ast);
    void add_net(verilog::Net&& net);
    void add_instance(verilog::Instance&& inst);

};