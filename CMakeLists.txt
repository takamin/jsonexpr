cmake_minimum_required(VERSION 2.8)
project( jsonexpr )
set(LIB_SOURCES
    jsonexpr.cpp
    parser.cpp
    )
set(TEST_SOURCES
    test/test.cpp
    )
set(TEST2_SOURCES
    test/readjson.cpp
    )
add_library(jsonexpr ${LIB_SOURCES})
add_executable(test ${TEST_SOURCES})
target_link_libraries(test jsonexpr)
add_executable(test2 ${TEST2_SOURCES})
target_link_libraries(test2 jsonexpr)
