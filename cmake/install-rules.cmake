include(CMakePackageConfigHelpers)
include(GNUInstallDirs)

set(package libgravix2)

install(
    FILES
    include/libgravix2/game.h
    ${PROJECT_BINARY_DIR}/api/libgravix2/api.h
    ${PROJECT_BINARY_DIR}/export/libgravix2/libgravix2_export.h
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/libgravix2
    COMPONENT libgravix2_Development
)

install(
    TARGETS libgravix2_libgravix2
    EXPORT libgravix2Targets
    RUNTIME
    COMPONENT libgravix2_Runtime
    LIBRARY
    COMPONENT libgravix2_Runtime
    NAMELINK_COMPONENT libgravix2_Development
    ARCHIVE
    COMPONENT libgravix2_Development
    INCLUDES
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)

write_basic_package_version_file(
    "${package}ConfigVersion.cmake"
    COMPATIBILITY SameMajorVersion
)

# Allow package maintainers to freely override the path for the configs
set(
    libgravix2_INSTALL_CMAKEDIR "${CMAKE_INSTALL_LIBDIR}/cmake/${package}"
    CACHE PATH "CMake package config location relative to the install prefix"
)
mark_as_advanced(libgravix2_INSTALL_CMAKEDIR)

install(
    FILES cmake/install-config.cmake
    DESTINATION "${libgravix2_INSTALL_CMAKEDIR}"
    RENAME "${package}Config.cmake"
    COMPONENT libgravix2_Development
)

install(
    FILES "${PROJECT_BINARY_DIR}/${package}ConfigVersion.cmake"
    DESTINATION "${libgravix2_INSTALL_CMAKEDIR}"
    COMPONENT libgravix2_Development
)

install(
    EXPORT libgravix2Targets
    NAMESPACE libgravix2::
    DESTINATION "${libgravix2_INSTALL_CMAKEDIR}"
    COMPONENT libgravix2_Development
)

if(PROJECT_IS_TOP_LEVEL)
    include(CPack)
endif()
