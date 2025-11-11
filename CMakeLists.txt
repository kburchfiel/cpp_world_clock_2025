cmake_minimum_required(VERSION 3.25)
project(cwc25)

# NOTE: I was not able to get this code to run successfully on Linux;
# therefore, I'm using a separate file (build/gpp_build_script.sh)
# to create the program directly via G++. This works fine, thankfully.
if(UNIX AND NOT APPLE)
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_executable(cwc25 cpp_world_clock.cpp)
endif()

if(WIN32)
set(CMAKE_CXX_STANDARD 23)
add_executable(cwc25 cpp_world_clock_wn.cpp)
endif()


