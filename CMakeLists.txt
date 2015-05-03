cmake_minimum_required(VERSION 2.8)
project( jsonXX )
set(LIB_SOURCES
    jsonXX.cpp
    parser.cpp
    )
set(TEST_SOURCES
    test/test.cpp
    )
add_library(jsonXX ${LIB_SOURCES})
add_executable(test jsonXX ${TEST_SOURCES})
target_link_libraries(test jsonXX)
