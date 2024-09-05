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

#pragma once

#include "../Cell.hpp"
#include "../CellLibrary.hpp"
#include <boost/spirit/home/x3.hpp>
#include <boost/spirit/home/x3/support/ast/variant.hpp>
#include <boost/container_hash/hash.hpp>
#include <string>
#include <algorithm>
#include <vector>
#include <functional>
#include <memory>
#include <unordered_map>

namespace liberty {

    struct SimpleAttribute {
        using Value = boost::variant<double, std::string, int>;
        std::string name;
        Value value;
    };

    struct ComplexAttribute {
        std::string name;
        std::vector<SimpleAttribute::Value> values;
    };

    enum class AttributeType {
        String,
        Boolean,
        Integer,
        FloatingPoint
    };

    struct DefineStatement {
        std::string attribute_name;
        std::string group_name;
        AttributeType attribute_type;
    };

    struct GroupStatement;
    using GroupStatementAst = boost::spirit::x3::forward_ast<GroupStatement>;
    struct AttributeStatement : boost::spirit::x3::variant<DefineStatement, SimpleAttribute, ComplexAttribute, GroupStatementAst> {
        using base_type::base_type;
        using base_type::operator=;
    };

    struct GroupStatement {
        std::string group_name;
        std::string name;
        std::vector<AttributeStatement> children;
    };

    using Library = GroupStatementAst;
    class CellLibraryBuilder {
    private:

        CellLibrary library;
        std::string currentCell;
        std::string currentPin;
        bool isInput;
        bool isSequentialCell;

        std::string findRelatedPin(const GroupStatementAst& group) const;
        bool isInputPin(const GroupStatementAst& group) const;
        bool isClockPin(const GroupStatementAst& group) const;
        LUT findLUT(const GroupStatementAst& group, const std::string& riseMatrixName, const std::string& fallMatrixName) const;
        LUT findInternalPower(const GroupStatementAst& group) const;
        LUT findDelay(const GroupStatementAst& group) const;
        LUT findOutputSlope(const GroupStatementAst& group) const;
        std::string findLeakageState(const GroupStatementAst& group) const;
        double findLeakage(const GroupStatementAst& group) const;
        std::string findOutputPinFunction(const GroupStatementAst& group) const;
        double findInputPinCapacitance(const GroupStatementAst& group) const;
        std::string findTimeUnit(const GroupStatementAst& group) const;
        std::string findVoltageUnit(const GroupStatementAst& group) const;
        std::string findCapacitanceUnit(const GroupStatementAst& group) const;
        std::string findLeakagePowerUnit(const GroupStatementAst& group) const;
        double findVoltage(const GroupStatementAst& group) const;

        std::vector<std::string> split(const std::string& str, const std::string& delimiter) const;
        std::vector<double> stringToDoubleVector(std::string str) const;
        std::vector<double> toDoubleVector(const std::vector<SimpleAttribute::Value>& value) const;
        std::vector<std::vector<double>> toDoubleMatrix(const std::vector<SimpleAttribute::Value>& vec, std::size_t x, std::size_t y) const;

    public:

        using result_type = void;
        result_type operator()(const GroupStatementAst&);
        result_type operator()(const SimpleAttribute&);
        result_type operator()(const ComplexAttribute&);
        result_type operator()(const DefineStatement&);

        CellLibrary getLibrary() const;

    };
}