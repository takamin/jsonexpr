cmake_minimum_required(VERSION 2.8)
project( jsonXX )
set(LIB_SOURCES
    jsonXX.cpp
    )
set(TEST_SOURCES
    test/test.cpp
    )
add_library(jsonXX ${LIB_SOURCES})
add_executable(test ${TEST_SOURCES} jsonXX)
