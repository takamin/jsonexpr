cmake_minimum_required(VERSION 2.8)
project( jexplus )
set(LIB_SOURCES
    jexplus.cpp
    parser.cpp
    )
set(TEST_SOURCES
    test/test.cpp
    )
set(TEST2_SOURCES
    test/readjson.cpp
    )
add_library(jexplus ${LIB_SOURCES})
add_executable(test ${TEST_SOURCES})
target_link_libraries(test jexplus)
add_executable(test2 ${TEST2_SOURCES})
target_link_libraries(test2 jexplus)
