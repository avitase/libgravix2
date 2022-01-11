add_library(gravix2 SHARED
    helpers.c
    integrators.c
    linalg.c
    missile.c
    planet.c
    pot.c
)
target_include_directories(gravix2 BEFORE PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(gravix2 PRIVATE project_warnings project_options m)
