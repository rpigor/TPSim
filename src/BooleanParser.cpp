#include "BooleanParser.hpp"
#include <iostream>
#include <stdexcept>

boost::tribool BooleanFunctionVisitor::operator()(const Variable& v) const {
    auto it = std::find(cellInputs.begin(), cellInputs.end(), v);
    auto idx = std::distance(cellInputs.begin(), it);
    return input.at(idx);
}

boost::tribool BooleanFunctionVisitor::operator()(const UnaryOperation<NotOp>& u) const {
    return !boost::apply_visitor(*this, u.leftOper);
}

boost::tribool BooleanFunctionVisitor::operator()(const BinaryOperation<OrOp>& b) const {
    auto leftRes = boost::apply_visitor(*this, b.leftOper);
    auto rightRes = boost::apply_visitor(*this, b.rightOper);
    return leftRes || rightRes;
}

boost::tribool BooleanFunctionVisitor::operator()(const BinaryOperation<AndOp>& b) const {
    auto leftRes = boost::apply_visitor(*this, b.leftOper);
    auto rightRes = boost::apply_visitor(*this, b.rightOper);
    return leftRes && rightRes;
}

boost::tribool BooleanFunctionVisitor::operator()(const BinaryOperation<XorOp>& b) const {
    auto leftRes = boost::apply_visitor(*this, b.leftOper);
    auto rightRes = boost::apply_visitor(*this, b.rightOper);
    return (leftRes || rightRes) && !(leftRes && rightRes);
}

boost::tribool BooleanFunctionVisitor::evaluateExpression(const Expression& expr, const std::vector<std::string>& cellInputs, const std::vector<boost::tribool>& input) {
    this->cellInputs = cellInputs;
    this->input = input;
    return boost::apply_visitor(*this, expr);
}