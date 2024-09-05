#include <catch2/catch_test_macros.hpp>
#include "../src/BooleanParser.hpp"
#include <boost/logic/tribool.hpp>
#include <string>
#include <vector>

TEST_CASE( "boolean parser works", "[boolean]" ) {

    std::vector<std::string> inputs{"S", "D0", "D1"};
    std::string func("(!S&D0)+(S&D1)");

    Expression result;
    BooleanParser<std::string::iterator> boolParser;
    auto firstIt(std::begin(func)), lastIt(std::end(func));
    REQUIRE(boost::spirit::qi::phrase_parse(firstIt, lastIt, boolParser, boost::spirit::qi::space, result));

    SECTION( "basic logic" ) {
        std::vector<boost::tribool> inVec{false, false, false};
        auto tb = BooleanFunctionVisitor().evaluateExpression(result, inputs, inVec);
        bool b = (tb == false) ? true : false;
        REQUIRE(b);

        inVec = {false, true, false};
        tb = BooleanFunctionVisitor().evaluateExpression(result, inputs, inVec);
        b = (tb == true) ? true : false;
        REQUIRE(b);

        inVec = {true, true, false};
        tb = BooleanFunctionVisitor().evaluateExpression(result, inputs, inVec);
        b = (tb == false) ? true : false;
        REQUIRE(b);

        inVec = {true, true, true};
        tb = BooleanFunctionVisitor().evaluateExpression(result, inputs, inVec);
        b = (tb == true) ? true : false;
        REQUIRE(b);

        inVec = {false, false, false, true, true, true, true};
        tb = BooleanFunctionVisitor().evaluateExpression(result, inputs, inVec);
        b = (tb == false) ? true : false;
        REQUIRE(b);
    }
    SECTION( "indeterminate logic" ) {
        std::vector<boost::tribool> inVec = {boost::indeterminate, false, true};
        auto tb = BooleanFunctionVisitor().evaluateExpression(result, inputs, inVec);
        REQUIRE(boost::indeterminate(tb));

        inVec = {true, boost::indeterminate, true};
        tb = BooleanFunctionVisitor().evaluateExpression(result, inputs, inVec);
        bool b = (tb == true) ? true : false;
        REQUIRE(b);
    }

}