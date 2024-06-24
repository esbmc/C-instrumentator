#################################
# Project Information           #
#################################
cmake_minimum_required(VERSION 3.7)
project(CInstrumentation)

#################################
# Configuration                 #
#################################

# Demand C++17
set (CMAKE_CXX_STANDARD 23)

# Adds custom modules from ESBMC and default Options
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/scripts/cmake/")


include(ExternalDependencies)

# Source Code

include_directories(include)
add_subdirectory(src)

if (BUILD_TESTING)
    add_subdirectory(unit)
endif ()
