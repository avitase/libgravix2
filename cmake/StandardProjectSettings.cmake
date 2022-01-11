if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build." FORCE)
	set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
		"Debug"
        "Release"
    )
endif ()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(ARCH "native" CACHE STRING "Optimization target for platform specific optimization")

option(ENABLE_PSO "Enable platform specific optimization" ON)
if (ENABLE_PSO)
    if (MSVC)
        message(SEND_ERROR "Platform specifig optimization not yet available for MSVC")
    else ()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=${ARCH}")
    endif ()
endif ()

option(ENABLE_IPO "Enable Interprocedural Optimization, aka Link Time Optimization (LTO)" ON)
if (ENABLE_IPO)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT result OUTPUT output)
    if (result)
        set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
    else ()
        message(SEND_ERROR "IPO is not supported: ${output}")
    endif ()
endif ()
