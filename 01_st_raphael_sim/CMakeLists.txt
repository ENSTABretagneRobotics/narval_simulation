cmake_minimum_required(VERSION 3.16)
project(st_raphael_sim)

# These will clone the needed dependencies using cmake FetchContent feature
# This will clone from github, so a working internet connection is required. 
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchRTAC.cmake)
FetchRTAC()

add_executable(01_st_raphael_sim
    src/main.cpp
)
target_link_libraries(01_st_raphael_sim PRIVATE
    rtac_simulation
)

