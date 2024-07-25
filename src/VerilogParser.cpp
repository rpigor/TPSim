#include "VerilogParser.hpp"
#include <algorithm>

void VerilogParser::add_module(std::string&& name) {
    module.name = std::move(name);
}

void VerilogParser::add_port(verilog::Port&& port) {
    if (port.dir == verilog::PortDirection::INPUT) {
        module.inputs.insert(module.inputs.end(), port.names.begin(), port.names.end());
    }
    else if (port.dir == verilog::PortDirection::OUTPUT) {
        module.outputs.insert(module.outputs.end(), port.names.begin(), port.names.end());
    }
    else {
        module.inputs.insert(module.inputs.end(), port.names.begin(), port.names.end());
        module.outputs.insert(module.outputs.end(), port.names.begin(), port.names.end());
    }
    std::move(port.names.begin(), port.names.end(), std::back_inserter(module.ports));
}

void VerilogParser::add_net(verilog::Net&& net) {
    if(net.type == verilog::NetType::WIRE){
        std::move(net.names.begin(), net.names.end(), std::back_inserter(module.wires));
    }
}

void VerilogParser::add_assignment(verilog::Assignment&& ast) {

}

void VerilogParser::add_instance(verilog::Instance&& inst) {
    auto &g = module.gates.emplace_back();
    g.cell = inst.module_name;
    g.name = inst.inst_name;
    const Cell& cell = lib.at(g.cell);

    std::string pin;
    std::string net;
    for (std::size_t i = 0; i < inst.net_names.size(); i++) {
        auto &net_name = inst.net_names[i][0];

        if (i < inst.net_names.size()) {
            auto &pin_name = inst.pin_names[i];
            switch (pin_name.index()) {
                case 0: pin = std::get<0>(pin_name); break;
                case 1: pin = std::get<1>(pin_name).name; break;
                case 2: pin = std::get<2>(pin_name).name; break;
            }

            switch (net_name.index()) {
                case 0: net = std::get<0>(net_name); break;
                case 1: net = std::get<1>(net_name).name; break;
                case 2: net = std::get<2>(net_name).name; break;
                case 3: net = std::get<3>(net_name).value; break;
            }

            if (std::find(cell.inputs.begin(), cell.inputs.end(), pin) != cell.inputs.end()) {
                g.input2net.insert({pin, net});
                g.net2input.insert({std::move(net), std::move(pin)});
            }
            else {
                g.output2net.insert({pin, net});
                g.net2output.insert({std::move(net), std::move(pin)});
            }
        }
    }
}