#pragma once

#include <boost/spirit/include/qi.hpp>
#include <boost/phoenix.hpp>
#include <boost/phoenix/operator.hpp>
#include <boost/variant/recursive_wrapper.hpp>
#include <boost/logic/tribool.hpp>
#include <string>
#include <vector>
#include <functional>

struct NotOp { };
struct OrOp  { };
struct AndOp { };
struct XorOp { };

template <typename Tag>
struct BinaryOperation;

template <typename Tag>
struct UnaryOperation;

typedef std::string Variable;

typedef boost::variant<
            Variable,
            boost::recursive_wrapper<UnaryOperation<NotOp>>,
            boost::recursive_wrapper<BinaryOperation<OrOp>>,
            boost::recursive_wrapper<BinaryOperation<AndOp>>,
            boost::recursive_wrapper<BinaryOperation<XorOp>>
        > Expression;

template <typename Tag>
struct BinaryOperation {

    Expression leftOper, rightOper;

    explicit BinaryOperation(const Expression& l, const Expression& r) : leftOper(l), rightOper(r) { }

};

template <typename Tag>
struct UnaryOperation {

    Expression leftOper;

    explicit UnaryOperation(const Expression& o) : leftOper(o) { }

};

template <typename It, typename Skipper = boost::spirit::qi::space_type>
struct BooleanParser : boost::spirit::qi::grammar<It, Expression(), Skipper> {

    BooleanParser() : BooleanParser::base_type(expression_) {
        using namespace boost::spirit::qi;
        namespace phx   = boost::phoenix;

        expression_  = or_.alias();

        or_  =
            and_ [ _val = _1 ] >> *(
                                        char_('|')  >> and_     [ _val = phx::construct<BinaryOperation<OrOp>> (_val, _1) ]
                                        | char_('+') >> and_    [ _val = phx::construct<BinaryOperation<OrOp>> (_val, _1) ]
                                    );

        and_ =
            xor_ [ _val = _1 ] >> *(
                                        char_('&') >> xor_      [ _val = phx::construct<BinaryOperation<AndOp>>(_val, _1) ]
                                        | char_('*') >> xor_    [ _val = phx::construct<BinaryOperation<AndOp>>(_val, _1) ]
                                    );

        xor_ =
            not_ [ _val = _1 ] >> *(
                                        char_('^') >> not_      [ _val = phx::construct<BinaryOperation<XorOp>>(_val, _1) ]
                                    );

        not_ =
            ( '!' > term_ )                                     [ _val = phx::construct<UnaryOperation<NotOp>>(_1) ]
            | term_                                             [ _val = _1 ];

        term_ =
            (
                ( '(' > expression_ > ')' )
                | var_
            );

        var_ = lexeme[ +alnum ];

        BOOST_SPIRIT_DEBUG_NODE(expression_);
        BOOST_SPIRIT_DEBUG_NODE(or_);
        BOOST_SPIRIT_DEBUG_NODE(and_);
        BOOST_SPIRIT_DEBUG_NODE(xor_);
        BOOST_SPIRIT_DEBUG_NODE(not_);
        BOOST_SPIRIT_DEBUG_NODE(term_);
        BOOST_SPIRIT_DEBUG_NODE(var_);
    }

private:

    boost::spirit::qi::rule<It, Variable() , Skipper> var_;
    boost::spirit::qi::rule<It, Expression(), Skipper> not_, and_, xor_, or_, term_, expression_;

};

typedef std::function<boost::tribool(const std::vector<boost::tribool>&)> BooleanFunction;

class BooleanFunctionVisitor : boost::static_visitor<boost::tribool> {
private:

    std::vector<std::string> cellInputs;
    std::vector<boost::tribool> input;

public:

    boost::tribool operator()(const Variable& v) const;
    boost::tribool operator()(const UnaryOperation<NotOp>& u) const;
    boost::tribool operator()(const BinaryOperation<OrOp >& b) const;
    boost::tribool operator()(const BinaryOperation<AndOp>& b) const;
    boost::tribool operator()(const BinaryOperation<XorOp>& b) const;

    static Expression parseExpression(std::string expression, const BooleanParser<std::string::iterator>& parser);
    boost::tribool evaluateExpression(const Expression& expr, const std::vector<std::string>& cellInputs, const std::vector<boost::tribool>& input);

};