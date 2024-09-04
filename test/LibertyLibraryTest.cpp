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

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>
#include <catch2/catch_approx.hpp>
#include "LibertyLibraryTest.hpp"
#include "../src/liberty/LibertyGrammar.hpp"
#include <boost/fusion/include/std_pair.hpp>
#include <vector>
#include <utility>

using namespace liberty;
using namespace spirit_test;

TEST_CASE("Name") {
    const char input[] = "library";
    std::string name;
    CHECK(test_attr(input, liberty::name, name));
    REQUIRE(name == "library");
}

TEST_CASE("Invalid name") {
    const char input[] = "4library";
    REQUIRE(test_failure(input, liberty::name));
}

TEST_CASE("Quoted string") {
    const char input[] = "\"quoted string \"";
    std::string name;
    CHECK(test_attr(input, liberty::quotedString, name));
    REQUIRE(name == "quoted string ");
}

TEST_CASE("Invalid quoted string") {
    const char input[] = "\"quoted string";
    REQUIRE(test_failure(input, liberty::quotedString));
}

TEST_CASE("Double value") {
    const char input[] = "4.2";
    liberty::SimpleAttribute::Value value;
    CHECK(test_attr(input, liberty::value, value));
    REQUIRE_NOTHROW(boost::get<double>(value) == Catch::Approx(4.2));
}

TEST_CASE("Integer value") {
    const char input[] = "4";
    liberty::SimpleAttribute::Value value;
    CHECK(test_attr(input, liberty::value, value));
    REQUIRE_NOTHROW(boost::get<int>(value) == 4);
}


TEST_CASE("Quoted string value") {
    const char input[] = "\"4.2\"";
    liberty::SimpleAttribute::Value value;
    CHECK(test_attr(input, liberty::value, value));
    REQUIRE_NOTHROW(boost::get<std::string>(value) == "4.2");
}

TEST_CASE("Non-quoted string value") {
    const char input[] = "1uW";
    liberty::SimpleAttribute::Value value;
    CHECK(test_attr(input, liberty::value, value));
    REQUIRE_NOTHROW(boost::get<std::string>(value) == "1uW");
}

TEST_CASE("Empty group statement") {
    const char input[] = "group_name (name) { }";
    liberty::GroupStatement group;
    INFO(input);
    CHECK(test_attr(input, liberty::groupStatement, group, liberty::x3::space));
    CHECK(group.group_name == "group_name");
    CHECK(group.name == "name");
    REQUIRE(group.children.empty());
}

TEST_CASE("Group statement without name") {
    const char input[] = "timing() {}";
    liberty::GroupStatement group;
    INFO(input);
    CHECK(test_attr(input, liberty::groupStatement, group, liberty::x3::space));
    CHECK(group.group_name == "timing");
    CHECK(group.name.empty());
}

TEST_CASE("Big group statement") {
    const char input[] = "bigGroupName (bigName) { attr : 42.0; innerComplex(1.0, 1, \"cmp lex\"); }";
    liberty::GroupStatement group;
    INFO(input);
    CHECK(test_attr(input, liberty::groupStatement, group, liberty::x3::space));
    CHECK(group.group_name == "bigGroupName");
    CHECK(group.name == "bigName");
    REQUIRE(group.children.size() == 2);
    liberty::SimpleAttribute simpleFirst;
    REQUIRE_NOTHROW(simpleFirst = boost::get<liberty::SimpleAttribute>(group.children.at(0)));
    CHECK(simpleFirst.name == "attr");
    CHECK(boost::get<double>(simpleFirst.value) == Catch::Approx(42.0));
    liberty::ComplexAttribute groupInner;
    REQUIRE_NOTHROW(groupInner = boost::get<liberty::ComplexAttribute>(group.children.at(1)));
    CHECK(groupInner.name == "innerComplex");
    REQUIRE(groupInner.values.size() == 3);
    double first;
    int second;
    std::string third;
    REQUIRE_NOTHROW(first = boost::get<double>(groupInner.values.at(0)));
    REQUIRE_NOTHROW(second = boost::get<int>(groupInner.values.at(1)));
    REQUIRE_NOTHROW(third = boost::get<std::string>(groupInner.values.at(2)));
    REQUIRE(first == Catch::Approx(1.0));
    REQUIRE(second == 1);
    REQUIRE(third == "cmp lex");
}

TEST_CASE("Simple double attribute") {
    const char input[] = "attr : 1.4 ;";
    liberty::SimpleAttribute attr;
    CHECK(test_attr(input, liberty::simpleAttribute, attr, liberty::x3::space));
    CHECK(attr.name == "attr");
    CHECK_NOTHROW(boost::get<double>(attr.value) == Catch::Approx(1.4));
}

TEST_CASE("Simple quoted string attribute") {
    const char input[] = "strattr : \"1.4\" ;";
    liberty::SimpleAttribute attr;
    CHECK(test_attr(input, liberty::simpleAttribute, attr, liberty::x3::space));
    CHECK(attr.name == "strattr");
    CHECK_NOTHROW(boost::get<std::string>(attr.value) == "1.4");
}

TEST_CASE("Simple non-quoted string attribute") {
    const char input[] = "nqstrattr : name ;";
    liberty::SimpleAttribute attr;
    CHECK(test_attr(input, liberty::simpleAttribute, attr, liberty::x3::space));
    CHECK(attr.name == "nqstrattr");
    CHECK_NOTHROW(boost::get<std::string>(attr.value) == "name");
}


TEST_CASE("Complex attribute") {
    const char input[] = "attr_name (val1, 1.2, \"va l3\", 1);";
    liberty::ComplexAttribute attr;
    CHECK(test_attr(input, liberty::complexAttribute, attr, liberty::x3::space));
    CHECK(attr.name == "attr_name");
    CHECK(attr.values.size() == 4);
    CHECK_NOTHROW(boost::get<std::string>(attr.values.at(0)) == "val1");
    CHECK_NOTHROW(boost::get<double>(attr.values.at(1)) == 1.2);
    CHECK_NOTHROW(boost::get<std::string>(attr.values.at(2)) == "va l3");
    CHECK_NOTHROW(boost::get<int>(attr.values.at(3)) == 1);
}

TEST_CASE("Attribute type") {
    // TODO: Set a default type for unrecognized values.
    const char input[] = "string integer Boolean float";
    std::vector<liberty::AttributeType> types;
    CHECK(test_attr(input, +liberty::attribute, types, liberty::x3::space));
    const std::vector<liberty::AttributeType> golden({
                                                     liberty::AttributeType::String,
                                                     liberty::AttributeType::Integer,
                                                     liberty::AttributeType::Boolean,
                                                     liberty::AttributeType::FloatingPoint
                                                 });
    REQUIRE(types == golden);
}

TEST_CASE("Define statement") {
    const char input[] = "define (attribute_name, group_name, string);";
    liberty::DefineStatement attr;
    CHECK(test_attr(input, liberty::defineStatement, attr, liberty::ascii::blank));
    CHECK(attr.attribute_name == "attribute_name");
    CHECK(attr.group_name == "group_name");
    CHECK(attr.attribute_type == liberty::AttributeType::String);
}

TEST_CASE("Variant attribute statement") {
    const char defineStatement[] = "define (attribute_name, group_name, Boolean);";
    const char simpleDoubleAttribute[] = "attr : 1.4 ;";
    const char complexAttribute[] = "attr_name (val1, 1.2, \"va l3\", 1);";
    const char groupStatement[] = "group_name (name) { }";

    liberty::AttributeStatement attr;
    CHECK(test_attr(defineStatement, liberty::attributeStatement, attr, liberty::x3::space));
    liberty::DefineStatement define;
    REQUIRE_NOTHROW(define = boost::get<liberty::DefineStatement>(attr));
    CHECK(define.attribute_name == "attribute_name");
    CHECK(define.group_name == "group_name");
    CHECK(define.attribute_type == liberty::AttributeType::Boolean);

    liberty::SimpleAttribute simple;
    CHECK(test_attr(simpleDoubleAttribute, liberty::attributeStatement, attr, liberty::x3::space));
    REQUIRE_NOTHROW(simple = boost::get<liberty::SimpleAttribute>(attr));
    CHECK(simple.name == "attr");
    CHECK_NOTHROW(boost::get<double>(simple.value) == Catch::Approx(1.4));

    liberty::ComplexAttribute complex;
    CHECK(test_attr(complexAttribute, liberty::attributeStatement, attr, liberty::x3::space));
    REQUIRE_NOTHROW(complex = boost::get<liberty::ComplexAttribute>(attr));
    CHECK(complex.name == "attr_name");
    CHECK(complex.values.size() == 4);
    CHECK_NOTHROW(boost::get<std::string>(complex.values.at(0)) == "val1");
    CHECK_NOTHROW(boost::get<double>(complex.values.at(1)) == 1.2);
    CHECK_NOTHROW(boost::get<std::string>(complex.values.at(2)) == "va l3");
    CHECK_NOTHROW(boost::get<int>(complex.values.at(3)) == 1);

    liberty::GroupStatementAst group;
    CHECK(test_attr(groupStatement, liberty::attributeStatement, attr, liberty::x3::space));
    REQUIRE_NOTHROW(group = boost::get<liberty::GroupStatementAst>(attr));
    CHECK(group.get().group_name == "group_name");
    CHECK(group.get().name == "name");
}

TEST_CASE("Empty attribute list") {
    const char input[] = "{ }";
    std::vector<liberty::AttributeStatement> list;
    CHECK(test_attr(input, liberty::attributeList, list, liberty::ascii::space));
    REQUIRE(list.empty());
}

TEST_CASE("Attribute list") {
    const char input[] =
                "{ simple : \"quoted str\" ; double : 4.2 ; }";
    std::vector<liberty::AttributeStatement> list;
    CHECK(test_attr(input, liberty::attributeList, list, liberty::ascii::space));
    REQUIRE(list.size() == 2);
    liberty::SimpleAttribute simple;
    CHECK_NOTHROW(simple = boost::get<liberty::SimpleAttribute>(list.at(0)));
    CHECK(simple.name == "simple");
    CHECK_NOTHROW(boost::get<std::string>(simple.value) == "quoted str");
    liberty::SimpleAttribute double_;
    CHECK_NOTHROW(double_ = boost::get<liberty::SimpleAttribute>(list.at(1)));
    CHECK(double_.name == "double");
    CHECK_NOTHROW(boost::get<double>(double_.value) == Catch::Approx(4.2));
}

TEST_CASE("Nested Group") {
    const char input[] = "group1(name1) { group2(name2) { attr: 4.2 ; } }";
    liberty::GroupStatement group;
    CHECK(test_attr(input, liberty::groupStatement, group, liberty::ascii::space));
    CHECK(group.group_name == "group1");
    CHECK(group.name == "name1");
    REQUIRE(group.children.size() == 1);
    liberty::GroupStatementAst innerGroup;
    REQUIRE_NOTHROW(innerGroup = boost::get<liberty::GroupStatementAst>(group.children.at(0)));
    CHECK(innerGroup.get().group_name == "group2");
    CHECK(innerGroup.get().name == "name2");
    REQUIRE(innerGroup.get().children.size() == 1);
    liberty::SimpleAttribute attr;
    REQUIRE_NOTHROW(attr = boost::get<liberty::SimpleAttribute>(innerGroup.get().children.at(0)));
    CHECK(attr.name == "attr");
    CHECK_NOTHROW(boost::get<double>(attr.value) == Catch::Approx(4.2));
}

TEST_CASE("Comment") {
    const char input[] = "1.1 /* SHOULD SKIP THIS */ \"quoted string\"";
    std::pair<double, std::string> out;
    CHECK(test_attr(input, (liberty::double_ >> liberty::stringValue), out, liberty::spaceComment));
    CHECK(out.first == Catch::Approx(1.1));
    CHECK(out.second == "quoted string");
}

TEST_CASE("Blank library with few comments") {
    const char input[] = "/*  * Library File Format  * Copyright (c) 2011, Synopsys, Inc. and others. All Rights reserved.  */ library (\"contest\") { }";
    liberty::GroupStatement library;
    CHECK(test_attr(input, liberty::groupStatement, library, liberty::spaceComment));
    CHECK(library.group_name == "library");
    CHECK(library.name == "contest");
}

TEST_CASE("Lookup Table") {
    const char input[] = "rise_transition (\"delay_outputslew_template_7X8\") {"
         "index_1 (\"0.00,1.00,2.00,4.00,8.00,16.00,32.00\") ;"
         "index_2 (\"5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00\") ;"
         "values ("
         "\"15, 16.464, 19.656, 24.12, 30.204, 34.38, 39.54, 47.796\","
         "\"22.5, 23.04, 25.38, 30.336, 38.58, 44.472, 51.576, 61.572\","
         "\"30, 30.108, 31.692, 35.892, 45.564, 52.908, 61.992, 74.232\","
         "\"45, 45, 45.384, 48.12, 57.192, 66.924, 79.368, 96.684\","
         "\"75, 75, 75, 75.48, 81.012, 89.712, 106.536, 132.276\","
         "\"135, 135, 135, 135, 135.672, 140.004, 152.304, 185.376\","
         "\"255, 255, 255, 255, 255, 255, 258.18, 278.508\""
         ");"
         "}";
    liberty::GroupStatement lut;
    CHECK(test_attr(input, liberty::groupStatement, lut, liberty::ascii::space));;
    CHECK(lut.group_name == "rise_transition");
    CHECK(lut.name == "delay_outputslew_template_7X8");
    REQUIRE(lut.children.size() == 3);
    liberty::ComplexAttribute index_1, index_2, values;
    CHECK_NOTHROW(index_1 = boost::get<liberty::ComplexAttribute>(lut.children.at(0)));
    CHECK_NOTHROW(index_2 = boost::get<liberty::ComplexAttribute>(lut.children.at(1)));
    CHECK_NOTHROW(values = boost::get<liberty::ComplexAttribute>(lut.children.at(2)));
    CHECK(index_1.name == "index_1");
    REQUIRE(index_1.values.size() == 1);
    CHECK_NOTHROW(boost::get<std::string>(index_1.values.front()) == "0.00,1.00,2.00,4.00,8.00,16.00,32.00");
    CHECK(index_2.name == "index_2");
    REQUIRE(index_2.values.size() == 1);
    CHECK_NOTHROW(boost::get<std::string>(index_2.values.front()) == "5.00,30.00,50.00,80.00,140.00,200.00,300.00,500.00");
    CHECK(values.name == "values");
    CHECK(values.values.size() == 7);
    const std::vector<std::string> golden = {
        {"15, 16.464, 19.656, 24.12, 30.204, 34.38, 39.54, 47.796",
         "22.5, 23.04, 25.38, 30.336, 38.58, 44.472, 51.576, 61.572",
         "30, 30.108, 31.692, 35.892, 45.564, 52.908, 61.992, 74.232",
         "45, 45, 45.384, 48.12, 57.192, 66.924, 79.368, 96.684",
         "75, 75, 75, 75.48, 81.012, 89.712, 106.536, 132.276",
         "135, 135, 135, 135, 135.672, 140.004, 152.304, 185.376",
         "255, 255, 255, 255, 255, 255, 258.18, 278.508"}};
    std::vector<std::string> extractedValues;
    std::transform(values.values.begin(), values.values.end(), std::back_inserter(extractedValues), [](auto& value){
        return boost::get<std::string>(value);
    });

    REQUIRE(golden == extractedValues);
}

ParsingApiFixture::ParsingApiFixture() {
    std::ifstream in(test_filename, std::fstream::in);
    std::copy(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>(), std::back_inserter(buffer));
    std::replace(buffer.begin(), buffer.end(), '\n', ' ');
    std::replace(buffer.begin(), buffer.end(), '\\', ' ');
    auto first = buffer.begin();
    result = liberty::liberty_parse(first, buffer.end(), library);
}

const char Sample2LateFilenameFixture::test_filename[] = LIBERTY_TEST_RESOURCE_PATH "/sample2_Late.lib";

TEST_CASE_METHOD(ParsingApiFixture, "Input file (API)", "[file]") {
    REQUIRE(result);
    CHECK(library.get().group_name == "library");
    CHECK(library.get().name == "contest");
}