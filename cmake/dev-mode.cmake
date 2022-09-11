option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    include(CTest)
    enable_testing()
    add_subdirectory(test)
endif()

option(ENABLE_COVERAGE "Enable coverage" OFF)
if(ENABLE_COVERAGE)
    include(cmake/coverage.cmake)
endif()

include(cmake/lint-targets.cmake)