cmake_minimum_required(VERSION 3.21)

project(libgravix2Examples C)

if (PROJECT_IS_TOP_LEVEL)
    find_package(libgravix2 REQUIRED)
endif ()

add_custom_target(run-examples)

function(add_example NAME)
    add_executable("${NAME}" "${NAME}.c")
    target_link_libraries("${NAME}" PRIVATE libgravix2::libgravix2 m)
    target_compile_features("${NAME}" PRIVATE c_std_11)
    add_custom_target("run_${NAME}" COMMAND "${NAME}" VERBATIM)
    add_dependencies("run_${NAME}" "${NAME}")
    add_dependencies(run-examples "run_${NAME}")
endfunction()

add_example(scrcl)
