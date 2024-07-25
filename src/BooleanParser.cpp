#include "BooleanParser.hpp"
#include <iostream>
#include <stdexcept>

BooleanFunction BooleanFunctionVisitor::operator()(const Variable& v) const {
    return [](const std::vector<bool>& in) {
        return in[0];
    };
}

BooleanFunction BooleanFunctionVisitor::operator()(const UnaryOperation<NotOp>& u) const {
    auto notFunct = [&](const std::vector<bool>& in) {
        auto res = boost::apply_visitor(*this, u.leftOper)({in[0]});
        return !res;
    };
    return notFunct;
}

BooleanFunction BooleanFunctionVisitor::operator()(const BinaryOperation<OrOp>& b) const {
    auto orFunct = [&](const std::vector<bool>& in) {
        auto leftRes = boost::apply_visitor(*this, b.leftOper)({in[0]});
        auto rightRes = boost::apply_visitor(*this, b.rightOper)({in[1]});
        return leftRes || rightRes;
    };
    return orFunct;
}

BooleanFunction BooleanFunctionVisitor::operator()(const BinaryOperation<AndOp>& b) const {
    auto andFunct = [&](const std::vector<bool>& in) {
        auto leftRes = boost::apply_visitor(*this, b.leftOper)({in[0]});
        auto rightRes = boost::apply_visitor(*this, b.rightOper)({in[1]});
        return leftRes && rightRes;
    };
    return andFunct;
}

BooleanFunction BooleanFunctionVisitor::operator()(const BinaryOperation<XorOp>& b) const {
    auto xorFunct = [&](const std::vector<bool>& in) {
        auto leftRes = boost::apply_visitor(*this, b.leftOper)({in[0]});
        auto rightRes = boost::apply_visitor(*this, b.rightOper)({in[1]});
        return leftRes ^ rightRes;
    };
    return xorFunct;
}