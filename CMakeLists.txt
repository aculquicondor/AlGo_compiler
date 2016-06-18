cmake_minimum_required(VERSION 3.5)
project(algo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG")

set(SOURCE_FILES main.cpp
        definitions.cpp definitions.h
        lexical_descriptor.cpp lexical_descriptor.h
        lexical_analyzer.cpp lexical_analyzer.h
        source_code.cpp source_code.h
        analyzer.cpp analyzer.h
        symbol_table.cpp symbol_table.h
        rule_context.cpp rule_context.h
        semantic_rules.h)
add_executable(algo ${SOURCE_FILES})

file(COPY
        productions.csv syntactic_table.csv
        DESTINATION ${PROJECT_BINARY_DIR}/)