#!/bin/sh
mkdir build
cd build
CMAKE_C_COMPILER_ENV_VAR="C:\\MinGW\\bin"
CMAKE_CXX_COMPILER="C:\\MinGW\\bin"
#CMAKE_CXX_COMPILER_ENV_VAR=${CXX}
#CMAKE_C_COMPILER_ENV_VAR=${CC}
cmake -G 'MinGW Makefiles' ..
cmake -G 'MinGW Makefiles' ..
