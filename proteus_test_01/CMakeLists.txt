cmake_minimum_required(VERSION 3.10)
project(optix_acoustics01 VERSION 0.1 LANGUAGES CUDA CXX)

# These will clone the needed dependencies using cmake FetchContent feature
# This will clone from github, so a working internet connection is required. 
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/FetchRTAC.cmake)
FetchRTAC()

add_executable(optix_acoustics02
    src/ProfileExamples.cpp
    src/optix_acoustics.cpp
)
target_link_libraries(optix_acoustics02 PRIVATE
    rtac_simulation
)
target_add_ptx(optix_acoustics02 
    CUDA_SOURCES src/optix_acoustics.cu
)
