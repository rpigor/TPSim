#include "BooleanParser.hpp"
#include <iostream>
#include <stdexcept>
#include <unordered_map>

BooleanFunction BooleanFunctionVisitor::operator()(const Variable& v) const {
    return [](const std::vector<boost::tribool>& in) {
        return in[0];
    };
}

BooleanFunction BooleanFunctionVisitor::operator()(const UnaryOperation<NotOp>& u) const {
    auto notFunct = [&](const std::vector<boost::tribool>& in) {
        auto res = boost::apply_visitor(*this, u.leftOper)({in[0]});
        return !res;
    };
    return notFunct;
}

BooleanFunction BooleanFunctionVisitor::operator()(const BinaryOperation<OrOp>& b) const {
    auto orFunct = [&](const std::vector<boost::tribool>& in) {
        auto leftRes = boost::apply_visitor(*this, b.leftOper)({in[0]});
        auto rightRes = boost::apply_visitor(*this, b.rightOper)({in[1]});
        return leftRes || rightRes;
    };
    return orFunct;
}

BooleanFunction BooleanFunctionVisitor::operator()(const BinaryOperation<AndOp>& b) const {
    auto andFunct = [&](const std::vector<boost::tribool>& in) {
        auto leftRes = boost::apply_visitor(*this, b.leftOper)({in[0]});
        auto rightRes = boost::apply_visitor(*this, b.rightOper)({in[1]});
        return leftRes && rightRes;
    };
    return andFunct;
}

BooleanFunction BooleanFunctionVisitor::operator()(const BinaryOperation<XorOp>& b) const {
    auto xorFunct = [&](const std::vector<boost::tribool>& in) {
        auto leftRes = boost::apply_visitor(*this, b.leftOper)({in[0]});
        auto rightRes = boost::apply_visitor(*this, b.rightOper)({in[1]});
        return (leftRes || rightRes) && !(leftRes && rightRes);
    };
    return xorFunct;
}

BooleanFunction BooleanFunctionVisitor::getBooleanFunction(const Expression& expr, std::string func, const std::vector<std::string>& inputs) {
    std::replace(func.begin(), func.end(), '(', ' '); // forgive me Father for I have sinned...
    std::replace(func.begin(), func.end(), ')', ' ');
    std::replace(func.begin(), func.end(), '!', ' ');
    std::replace(func.begin(), func.end(), '*', ' ');
    std::replace(func.begin(), func.end(), '&', ' ');
    std::replace(func.begin(), func.end(), '|', ' ');
    std::replace(func.begin(), func.end(), '+', ' ');
    std::replace(func.begin(), func.end(), '^', ' ');

    int argIdx = 0;
    std::string inputBuf;
    std::unordered_map<std::string, std::vector<int>> inputArgumentIdxs;
    for (const auto& c : func) {
        if (::isspace(c)) {
            if (!inputBuf.empty()) {
                inputArgumentIdxs[inputBuf].push_back(argIdx++);
                inputBuf = "";
            }
            continue;
        }
        inputBuf.push_back(c);
    }
    if (!inputBuf.empty()) {
        inputArgumentIdxs[inputBuf].push_back(argIdx++);
    }

    auto f = boost::apply_visitor(BooleanFunctionVisitor(), expr);
    auto newF = [inputs,argIdx,inputArgumentIdxs,f](const std::vector<boost::tribool>& newIn) {
        std::vector<boost::tribool> completeIn(argIdx, boost::indeterminate);
        int newInIdx = 0;
        for (const auto& input : inputs) {
            for (int i : inputArgumentIdxs.at(input)) {
                completeIn[i] = newIn[newInIdx];
            }
            newInIdx++;
        }
        return f(completeIn);
    };
    return newF;
}