cmake_minimum_required(VERSION 3.27)

project(TPSim LANGUAGES CXX)

set(CMAKE_MESSAGE_LOG_LEVEL "STATUS")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(
  -Wall
  -Wextra
  -O3
  -Wno-unused-parameter
  -Wno-unused-value
  -Wno-missing-field-initializers
  -Wno-narrowing
  -Wno-pointer-arith)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/verilog)
include_directories(${PROJECT_SOURCE_DIR}/src/liberty)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(Boost 1.80.0 COMPONENTS program_options REQUIRED)
find_package(Catch2 REQUIRED)

BISON_TARGET(verilog_parser
             ${PROJECT_SOURCE_DIR}/src/verilog/verilog_parser.yy
             ${CMAKE_CURRENT_BINARY_DIR}/verilog_parser.tab.cc)
FLEX_TARGET(verilog_lexer
            ${PROJECT_SOURCE_DIR}/src/verilog/verilog_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/verilog_lexer.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(verilog_lexer verilog_parser)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

file(GLOB_RECURSE SRCFILES "src/*.cpp")
file(GLOB_RECURSE LIBERTYFILES "src/liberty/*.cpp")
add_executable(${PROJECT_NAME}
  ${SRCFILES}
  ${LIBERTYFILES}
  ${FLEX_verilog_lexer_OUTPUTS}
  ${BISON_verilog_parser_OUTPUTS}
)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "tpsim")
target_link_libraries(${PROJECT_NAME} Boost::boost Boost::program_options)


file(GLOB_RECURSE TESTFILES "test/*.cpp")
add_executable(TPSimTest
                ${TESTFILES}
                ${PROJECT_SOURCE_DIR}/src/Estimator.cpp
                ${PROJECT_SOURCE_DIR}/src/Units.cpp
                ${PROJECT_SOURCE_DIR}/src/EventQueue.cpp
                ${PROJECT_SOURCE_DIR}/src/liberty/LibertyLibrary.cpp)
set_target_properties(TPSimTest PROPERTIES OUTPUT_NAME "test")
target_compile_definitions(TPSimTest PUBLIC LIBERTY_TEST_RESOURCE_PATH="${PROJECT_SOURCE_DIR}/test")
target_link_libraries(TPSimTest PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)

catch_discover_tests(TPSimTest)