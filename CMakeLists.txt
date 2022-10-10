cmake_minimum_required(VERSION 3.21)

include(cmake/prelude.cmake)

project(
    libgravix2
    VERSION 0.1.0
    DESCRIPTION "A fast simulation of attractive forces acting on point-like particles embedded onto the surface of a sphere."
    HOMEPAGE_URL "https://github.com/avitase/libgravix2"
    LANGUAGES C
)

include(cmake/variables.cmake)

# ---- Declare library ----

add_library(
    libgravix2_libgravix2
    src/config.c
    src/game.c
    src/helpers.c
    src/integrators.c
    src/linalg.c
    src/missile.c
    src/observations.c
    src/planet.c
    src/pot.c
    src/version.c
)
add_library(libgravix2::libgravix2 ALIAS libgravix2_libgravix2)

include(GenerateExportHeader)
generate_export_header(
    libgravix2_libgravix2
    BASE_NAME libgravix2
    EXPORT_MACRO_NAME GRVX_EXPORT
    EXPORT_FILE_NAME export/libgravix2/libgravix2_export.h
)

if(NOT BUILD_SHARED_LIBS)
    target_compile_definitions(libgravix2_libgravix2 PUBLIC LIBGRAVIX2_STATIC_DEFINE)
endif()

set_target_properties(
    libgravix2_libgravix2 PROPERTIES
    C_VISIBILITY_PRESET hidden
    VERSION "${PROJECT_VERSION}"
    SOVERSION "${PROJECT_VERSION_MAJOR}"
    EXPORT_NAME libgravix2
    OUTPUT_NAME gravix2
)

target_include_directories(
    libgravix2_libgravix2 ${warning_guard}
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/api>"
    PRIVATE
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/config>"
)

target_include_directories(
    libgravix2_libgravix2 SYSTEM
    PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/export>"
)

target_link_libraries(libgravix2_libgravix2 PRIVATE m)

target_compile_features(libgravix2_libgravix2 PUBLIC c_std_11)

# ---- Install rules ----

if(NOT CMAKE_SKIP_INSTALL_RULES)
    include(cmake/install-rules.cmake)
endif()

# ---- Examples ----

if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_EXAMPLES "Build examples tree" "${libgravix2_DEVELOPER_MODE}")
    if(BUILD_EXAMPLES)
        add_subdirectory(examples)
    endif()
endif()

# ---- Documentation ----

if(PROJECT_IS_TOP_LEVEL)
    option(BUILD_DOCS "Build documentation using Doxygen" OFF)
    if(BUILD_DOCS)
        include(cmake/docs.cmake)
    endif()
endif()

# ---- Developer mode ----

if(libgravix2_DEVELOPER_MODE)
    if(PROJECT_IS_TOP_LEVEL)
        include(cmake/dev-mode.cmake)
    else()
        message(AUTHOR_WARNING "Developer mode is intended for developers of libgravix2")
    endif()
endif()

# ---- Config ----

include(config.cmake)
configure_file(${PROJECT_SOURCE_DIR}/config.h.in config/libgravix2/config.h @ONLY)
configure_file(${PROJECT_SOURCE_DIR}/api.h.in api/libgravix2/api.h @ONLY)
