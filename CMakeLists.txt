cmake_minimum_required(VERSION 3.5)
project(algo)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp
        definitions.cpp definitions.h
        lexical_descriptor.cpp lexical_descriptor.h
        lexical_analyzer.cpp lexical_analyzer.h
        source_code.cpp source_code.h)
add_executable(algo ${SOURCE_FILES})