set(GRVX_TRAJECTORY_SIZE "100" CACHE STRING "Size of trajectory")
set(GRVX_INT_STEPS "10" CACHE STRING "Number of integration steps between consecutive trajectory points")
set(GRVX_MIN_DIST "0.02" CACHE STRING "Min. allowed distance between missiles and planets")

set(GRVX_POT_TYPE "2D" CACHE STRING "Type of potential")
set(GRVX_POT_TYPE_VALUES "2D" "3D")
set_property(CACHE GRVX_POT_TYPE PROPERTY STRINGS ${GRVX_POT_TYPE_VALUES})
list(FIND GRVX_POT_TYPE_VALUES ${GRVX_POT_TYPE} GRVX_POT_TYPE_INDEX)
if(${GRVX_POT_TYPE_INDEX} EQUAL -1)
    message(FATAL_ERROR "Unkown potential type '${GRVX_POT_TYPE}'; supported entries are ${GRVX_POT_TYPE_VALUES}")
endif()

if("${GRVX_POT_TYPE}" STREQUAL "3D")
    set(GRVX_N_POT "2" CACHE STRING "Approximation order of potential")
else()
    set(GRVX_N_POT "???")
endif()

set(GRVX_COMPOSITION_SCHEME "p8s15" CACHE STRING "Composition method of integrator")
set_property(CACHE GRVX_COMPOSITION_SCHEME PROPERTY STRINGS "p2s1" "p4s3" "p4s5" "p6s9" "p8s15")
if("${GRVX_COMPOSITION_SCHEME}" STREQUAL "p2s1")
    set(GRVX_COMPOSITION_ID "0")
    set(GRVX_COMPOSITION_STAGES "1")
elseif("${GRVX_COMPOSITION_SCHEME}" STREQUAL "p4s3")
    set(GRVX_COMPOSITION_ID "1")
    set(GRVX_COMPOSITION_STAGES "3")
elseif("${GRVX_COMPOSITION_SCHEME}" STREQUAL "p4s5")
    set(GRVX_COMPOSITION_ID "2")
    set(GRVX_COMPOSITION_STAGES "5")
elseif("${GRVX_COMPOSITION_SCHEME}" STREQUAL "p6s9")
    set(GRVX_COMPOSITION_ID "3")
    set(GRVX_COMPOSITION_STAGES "9")
elseif("${GRVX_COMPOSITION_SCHEME}" STREQUAL "p8s15")
    set(GRVX_COMPOSITION_ID "4")
    set(GRVX_COMPOSITION_STAGES "15")
else()
    message(FATAL_ERROR "Unkown composition method '${GRVX_COMPOSITION_SCHEME}'")
endif()