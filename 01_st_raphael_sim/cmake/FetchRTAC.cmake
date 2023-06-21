function(FetchRTAC)

    set(multiValueArgs RTAC_BASE_VERSION RTAC_DISPLAY_VERSION RTAC_OPTIX_VERSION RTAC_SIMULATION_VERSION RTAC_DEFAULT_VERSION)
    cmake_parse_arguments(ARGUMENT "" "" "${multiValueArgs}" ${ARGN})

    if(NOT ARGUMENT_RTAC_DEFAULT_VERSION)
        set(RTAC_DEFAULT_VERSION "v1.0")
    endif()
    set(RTAC_BASE_VERSION       ${RTAC_DEFAULT_VERSION})
    set(RTAC_DISPLAY_VERSION    ${RTAC_DEFAULT_VERSION})
    set(RTAC_OPTIX_VERSION      ${RTAC_DEFAULT_VERSION})
    set(RTAC_SIMULATION_VERSION ${RTAC_DEFAULT_VERSION})
    set(RTAC_DEFAULT_VERSION    ${RTAC_DEFAULT_VERSION})

    if(ARGUMENT_RTAC_BASE_VERSION)
        set(RTAC_BASE_VERSION ${ARGUMENT_RTAC_BASE_VERSION})
    endif()
    if(ARGUMENT_RTAC_DISPLAY_VERSION)
        set(RTAC_DISPLAY_VERSION ${ARGUMENT_RTAC_DISPLAY_VERSION})
    endif()
    if(ARGUMENT_RTAC_OPTIX_VERSION)
        set(RTAC_OPTIX_VERSION ${ARGUMENT_RTAC_OPTIX_VERSION})
    endif()
    if(ARGUMENT_RTAC_SIMULATION_VERSION)
        set(RTAC_SIMULATION_VERSION ${ARGUMENT_RTAC_SIMULATION_VERSION})
    endif()

    message(STATUS "RTAC_DEFAULT_VERSION    : ${RTAC_DEFAULT_VERSION}")
    message(STATUS "RTAC_BASE_VERSION       : ${RTAC_BASE_VERSION}")
    message(STATUS "RTAC_DISPLAY_VERSION    : ${RTAC_DISPLAY_VERSION}")
    message(STATUS "RTAC_OPTIX_VERSION      : ${RTAC_OPTIX_VERSION}")
    message(STATUS "RTAC_SIMULATION_VERSION : ${RTAC_SIMULATION_VERSION}")

    if(NOT TARGET rtac_base)
        include(FetchContent)
        FetchContent_Declare(rtac_base
            GIT_REPOSITORY https://github.com/ENSTABretagneRobotics/rtac_base.git
            GIT_TAG        ${RTAC_BASE_VERSION}
        )
        FetchContent_MakeAvailable(rtac_base)
    endif()
    
    if(NOT TARGET rtac_display)
        include(FetchContent)
        FetchContent_Declare(rtac_display
            GIT_REPOSITORY https://github.com/ENSTABretagneRobotics/rtac_display.git
            GIT_TAG        ${RTAC_DISPLAY_VERSION}
        )
        FetchContent_MakeAvailable(rtac_display)
    endif()
    
    if(NOT TARGET rtac_optix)
        include(FetchContent)
        FetchContent_Declare(rtac_optix
            GIT_REPOSITORY https://github.com/ENSTABretagneRobotics/rtac_optix.git
            GIT_TAG        ${RTAC_OPTIX_VERSION}
        )
        FetchContent_MakeAvailable(rtac_optix)
    endif()
    
    if(NOT TARGET rtac_simulation)
        include(FetchContent)
        FetchContent_Declare(rtac_simulation
            GIT_REPOSITORY https://github.com/ENSTABretagneRobotics/rtac_simulation.git
            GIT_TAG        ${RTAC_SIMULATION_VERSION}
        )
        FetchContent_MakeAvailable(rtac_simulation)
    endif()

endfunction()