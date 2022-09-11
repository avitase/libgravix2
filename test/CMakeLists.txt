cmake_minimum_required(VERSION 3.21)

project(libgravix2Tests LANGUAGES CXX)

find_package(Catch2 REQUIRED 2)

add_executable(tests
    main.cpp
    test_config.cpp
    test_helpers.cpp
    test_integrator.cpp
    test_missile.cpp
    test_scrcl.cpp
    test_torb.cpp
)
set_property(TARGET tests PROPERTY CXX_STANDARD 20)
target_link_libraries(tests PRIVATE Catch2::Catch2 libgravix2::libgravix2)

if(ENABLE_SANITIZER)
    target_compile_options(tests PRIVATE -fsanitize=address,undefined)
    target_link_options(tests PRIVATE -fsanitize=address,undefined)
endif()

include(../config.cmake)
get_filename_component(parent_dir ${PROJECT_SOURCE_DIR} DIRECTORY)
configure_file(${parent_dir}/config.h.in config/config.h @ONLY)
target_include_directories(tests PRIVATE ${PROJECT_BINARY_DIR}/config)

include(Catch)
catch_discover_tests(tests)
