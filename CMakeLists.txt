cmake_minimum_required(VERSION 3.10)

project(TPSim LANGUAGES CXX)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O2")

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/src/verilog)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

find_package(BISON REQUIRED)
find_package(FLEX REQUIRED)
find_package(Boost 1.80.0 COMPONENTS program_options REQUIRED)

BISON_TARGET(verilog_parser
             ${PROJECT_SOURCE_DIR}/src/verilog/verilog_parser.yy
             ${CMAKE_CURRENT_BINARY_DIR}/verilog_parser.tab.cc)
FLEX_TARGET(verilog_lexer
            ${PROJECT_SOURCE_DIR}/src/verilog/verilog_lexer.l
            ${CMAKE_CURRENT_BINARY_DIR}/verilog_lexer.yy.cc)
ADD_FLEX_BISON_DEPENDENCY(verilog_lexer verilog_parser)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR})

add_executable(${PROJECT_NAME}
  ${PROJECT_SOURCE_DIR}/src/main.cpp ${PROJECT_SOURCE_DIR}/src/Simulator.cpp ${PROJECT_SOURCE_DIR}/src/VerilogParser.cpp ${PROJECT_SOURCE_DIR}/src/BooleanParser.cpp ${PROJECT_SOURCE_DIR}/src/VCD.cpp
  ${FLEX_verilog_lexer_OUTPUTS}
  ${BISON_verilog_parser_OUTPUTS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "tpsim")

target_link_libraries(${PROJECT_NAME} Boost::program_options)