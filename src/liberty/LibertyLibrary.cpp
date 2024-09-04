/*
Copyright 2017 Liberty Parser

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "LibertyLibrary.hpp"

namespace liberty {

    CellLibraryBuilder::result_type CellLibraryBuilder::operator()(const GroupStatementAst& group) {
        if (group.get().group_name == "library") {
            library.name = group.get().name;
            library.timeUnit = findTimeUnit(group);
            library.voltageUnit = findVoltageUnit(group);
            library.capacitanceUnit = findCapacitanceUnit(group);
            library.leakagePowerUnit = findLeakagePowerUnit(group);
        }
        else if (group.get().group_name == "operating_conditions") {
            library.voltage = findVoltage(group);
        }
        else if (group.get().group_name == "cell") {
            currentCell = group.get().name;

            Cell libCell;
            libCell.name = currentCell;
            library.cells[currentCell] = libCell;
        }
        else if (group.get().group_name == "internal_power" && !isInput) {
            Arc arc{findRelatedPin(group), currentPin};
            library.cells[currentCell].internalPower[arc] = findInternalPower(group);
        }
        else if (group.get().group_name == "timing" && !isInput) {
            Arc arc{findRelatedPin(group), currentPin};
            library.cells[currentCell].delay[arc] = findDelay(group);
            library.cells[currentCell].outputSlope[arc] = findOutputSlope(group);
        }
        else if (group.get().group_name == "leakage_power") {
            library.cells[currentCell].leakage[findLeakageState(group)] = findLeakage(group);
        }
        else if (group.get().group_name == "pin") {
            currentPin = group.get().name;
            if (isInputPin(group)) {
                isInput = true;
                library.cells[currentCell].inputs.push_back(currentPin);
                library.cells[currentCell].pinCapacitance[currentPin] = findInputPinCapacitance(group);
            }
            else {
                isInput = false;
                library.cells[currentCell].outputs.push_back(currentPin);
                library.cells[currentCell].bitFunctions.push_back(findOutputPinFunction(group));
            }
        }

        for (AttributeStatement const& child : group.get().children) {
            boost::apply_visitor(*this, child);
        }
    }

    CellLibraryBuilder::result_type CellLibraryBuilder::operator()(const SimpleAttribute& a)
    {

    }

    CellLibraryBuilder::result_type CellLibraryBuilder::operator()(const ComplexAttribute& a)
    {

    }

    CellLibraryBuilder::result_type CellLibraryBuilder::operator()(const DefineStatement& s)
    {

    }

    CellLibrary CellLibraryBuilder::getLibrary() const {
        return library;
    }

    std::vector<std::string> CellLibraryBuilder::split(const std::string& str, const std::string& delimiter) const {
        std::size_t pos_start = 0, pos_end, delim_len = delimiter.length();
        std::vector<std::string> res;
        std::string token;
        while ((pos_end = str.find(delimiter, pos_start)) != std::string::npos) {
            token = str.substr (pos_start, pos_end - pos_start);
            pos_start = pos_end + delim_len;
            res.push_back (token);
        }
        res.push_back (str.substr(pos_start));
        return res;
    }

    std::vector<double> CellLibraryBuilder::stringToDoubleVector(std::string str) const {
        std::vector<double> res;
        str.erase(std::remove_if(str.begin(), str.end(), isspace), str.end());
        for (const auto& s : split(str, ",")) {
            res.push_back(std::stod(s));
        }
        return res;
    }

    std::vector<double> CellLibraryBuilder::toDoubleVector(const std::vector<SimpleAttribute::Value>& value) const {
        std::vector<double> res;
        for (const auto& str : value) {
            auto tmp = stringToDoubleVector(boost::get<std::string>(str));
            res.insert(res.end(), tmp.begin(), tmp.end());
        }
        return res;
    }

    std::vector<std::vector<double>> CellLibraryBuilder::toDoubleMatrix(const std::vector<SimpleAttribute::Value>& vec, std::size_t x, std::size_t y) const {
        auto doubleVec = toDoubleVector(vec);
        int vecIdx = 0;
        std::vector<std::vector<double>> matrix(x, std::vector<double>(y, 0.0));
        for (std::size_t i = 0; i < x; i++)  {
            for (std::size_t j = 0; j < y; j++)  {
                matrix[i][j] = doubleVec[vecIdx];
                vecIdx++;
            }
        }
        return matrix;
    }

    std::string CellLibraryBuilder::findRelatedPin(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "related_pin") {
                return boost::get<std::string>(tmp.value);
            }
        }
        return "";
    }

    bool CellLibraryBuilder::isInputPin(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "direction") {
                return (boost::get<std::string>(tmp.value) == "input");
            }
        }
        return false;
    }

    LUT CellLibraryBuilder::findLUT(const GroupStatementAst& group, const std::string& riseMatrixName, const std::string& fallMatrixName) const {
        LUT lut;
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(GroupStatementAst)) {
                continue;
            }
            GroupStatementAst tmp = boost::get<GroupStatementAst>(c.get());
            if (tmp.get().children.size() != 3) {
                continue;
            }
            if (tmp.get().group_name == riseMatrixName) {
                auto x = boost::get<ComplexAttribute>(tmp.get().children.at(0));
                auto y = boost::get<ComplexAttribute>(tmp.get().children.at(1));
                auto z = boost::get<ComplexAttribute>(tmp.get().children.at(2));
                lut.xIdxValues = toDoubleVector(x.values);
                lut.yIdxValues = toDoubleVector(y.values);
                lut.riseZMatrix = toDoubleMatrix(z.values, lut.xIdxValues.size(), lut.xIdxValues.size());
            }
            else if (tmp.get().group_name == fallMatrixName) {
                auto x = boost::get<ComplexAttribute>(tmp.get().children.at(0));
                auto y = boost::get<ComplexAttribute>(tmp.get().children.at(1));
                auto z = boost::get<ComplexAttribute>(tmp.get().children.at(2));
                lut.xIdxValues = toDoubleVector(x.values);
                lut.yIdxValues = toDoubleVector(y.values);
                lut.fallZMatrix = toDoubleMatrix(z.values, lut.xIdxValues.size(), lut.xIdxValues.size());
            }
        }
        return lut;
    }

    LUT CellLibraryBuilder::findInternalPower(const GroupStatementAst& group) const {
        return findLUT(group, "rise_power", "fall_power");
    }

    LUT CellLibraryBuilder::findDelay(const GroupStatementAst& group) const {
        return findLUT(group, "cell_rise", "cell_fall");
    }

    LUT CellLibraryBuilder::findOutputSlope(const GroupStatementAst& group) const {
        return findLUT(group, "rise_transition", "fall_transition");
    }

    std::string CellLibraryBuilder::findLeakageState(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "when") {
                return boost::get<std::string>(tmp.value);
            }
        }
        return "";
    }

    double CellLibraryBuilder::findLeakage(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "value") {
                return boost::get<double>(tmp.value);
            }
        }
        return 0.0;
    }

    std::string CellLibraryBuilder::findOutputPinFunction(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "function") {
                return boost::get<std::string>(tmp.value);
            }
        }
        return "";
    }

    double CellLibraryBuilder::findInputPinCapacitance(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "capacitance") {
                return boost::get<double>(tmp.value);
            }
        }
        return 0.0;
    }

    std::string CellLibraryBuilder::findTimeUnit(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "time_unit") {
                std::string unit = boost::get<std::string>(tmp.value);
                unit.erase(std::remove_if(unit.begin(), unit.end(), ::isdigit), unit.end());
                return unit;
            }
        }
        return "";
    }

    std::string CellLibraryBuilder::findVoltageUnit(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "voltage_unit") {
                std::string unit = boost::get<std::string>(tmp.value);
                unit.erase(std::remove_if(unit.begin(), unit.end(), ::isdigit), unit.end());
                return unit;
            }
        }
        return "";
    }

    std::string CellLibraryBuilder::findCapacitanceUnit(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(ComplexAttribute)) {
                continue;
            }
            ComplexAttribute tmp = boost::get<ComplexAttribute>(c.get());
            if (tmp.name == "capacitive_load_unit") {
                return boost::get<std::string>(tmp.values[1]);
            }
        }
        return "";
    }

    std::string CellLibraryBuilder::findLeakagePowerUnit(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "leakage_power_unit") {
                std::string unit = boost::get<std::string>(tmp.value);
                unit.erase(std::remove_if(unit.begin(), unit.end(), ::isdigit), unit.end());
                return unit;
            }
        }
        return "";
    }

    double CellLibraryBuilder::findVoltage(const GroupStatementAst& group) const {
        for (const auto& c : group.get().children) {
            if (c.get().type() != typeid(SimpleAttribute)) {
                continue;
            }
            SimpleAttribute tmp = boost::get<SimpleAttribute>(c.get());
            if (tmp.name == "voltage") {
                return boost::get<double>(tmp.value);
            }
        }
        return 0.0;
    }

}
