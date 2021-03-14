# CMakeList.txt : CMake project for R18Z4Sim, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)
project(HardwareInloopSim)
# Add source to this project's executable.
add_executable (R18Z4Sim "R18Z4Sim.cpp" "includes/components.cpp" "includes/ipc.cpp" "R18Z4Sim.h")

# TODO: Add tests and install targets if needed.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")