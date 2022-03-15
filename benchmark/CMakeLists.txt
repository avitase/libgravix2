add_executable(scrcl
    scrcl.c
    ${PROJECT_SOURCE_DIR}/src/helpers.c
    ${PROJECT_SOURCE_DIR}/src/integrators.c
    ${PROJECT_SOURCE_DIR}/src/linalg.c
    ${PROJECT_SOURCE_DIR}/src/missile.c
    ${PROJECT_SOURCE_DIR}/src/planet.c
    ${PROJECT_SOURCE_DIR}/src/pot.c
)
target_include_directories(scrcl BEFORE PUBLIC
    ${CMAKE_CURRENT_BINARY_DIR}
    ${PROJECT_SOURCE_DIR}/include
)
target_link_libraries(scrcl PRIVATE project_warnings project_options m)