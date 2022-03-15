cmake_minimum_required(VERSION 3.18)

project(libgravix2 C)

include(cmake/StandardProjectSettings.cmake)
include(cmake/PreventInSourceBuilds.cmake)

add_library(project_options INTERFACE)
target_compile_features(project_options INTERFACE c_std_11)

add_library(project_warnings INTERFACE)
include(cmake/CompilerWarnings.cmake)
set_project_warnings(project_warnings)

include(cmake/Linker.cmake)
configure_linker(project_options)

include(cmake/Sanitizers.cmake)
enable_sanitizers(project_options)

include(cmake/Doxygen.cmake)
enable_doxygen()

option(ENABLE_TESTING "Enable Test Builds" OFF)
if (ENABLE_TESTING)
    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        enable_testing()
        add_subdirectory(test)
    else ()
        message(FATAL_ERROR "Unit test have to be build in debug mode")
    endif ()
endif ()

option(ENABLE_BENCHMARKS "Enable benchmarks" ON)
if (ENABLE_BENCHMARKS)
    add_subdirectory(benchmark)
endif ()

add_subdirectory(src)

set(POT_TYPE "2D" CACHE STRING "Type of potential")
set(POT_TYPE_VALUES "2D" "3D")
set_property(CACHE POT_TYPE PROPERTY STRINGS ${POT_TYPE_VALUES})
list(FIND POT_TYPE_VALUES ${POT_TYPE} POT_TYPE_INDEX)
if (${POT_TYPE_INDEX} EQUAL -1)
    message(FATAL_ERROR "Unkown potential type '${POT_TYPE}'; supported entries are ${POT_TYPE_VALUES}")
else ()
    message(STATUS "Using potential type: ${POT_TYPE}")
endif ()

if ("${POT_TYPE}" STREQUAL "3D")
    set(N_POT "2" CACHE STRING "Approximation order of potential")
endif()

set(TRAJECTORY_SIZE "100" CACHE STRING "Size of trajectory")
set(INT_STEPS "10" CACHE STRING "Number of integration steps between consecutive trajectory points")
set(MIN_DIST "0.02" CACHE STRING "Min. allowed distance between missiles and planets")
set(P_MIN "0.0001" CACHE STRING "Smallest possible momentum")

set(COMPOSITION_SCHEME "p8s15" CACHE STRING "Composition method of integrator")
set(COMPOSITION_SCHEME_VALUES "p2s1" "p4s3" "p4s5" "p6s9" "p8s15")
set_property(CACHE COMPOSITION_SCHEME PROPERTY STRINGS ${COMPOSITION_SCHEME_VALUES})
list(FIND COMPOSITION_SCHEME_VALUES ${COMPOSITION_SCHEME} COMPOSITION_SCHEME_INDEX)
if (${COMPOSITION_SCHEME_INDEX} EQUAL -1)
    message(FATAL_ERROR "Unkown composition method '${COMPOSITION_SCHEME}'; supported entries are ${COMPOSITION_SCHEME_VALUES}")
else ()
    message(STATUS "Using composition method: ${COMPOSITION_SCHEME}")
endif ()


configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in src/config.h @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in benchmark/config.h @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/include/config.h.in test/config.h @ONLY)

