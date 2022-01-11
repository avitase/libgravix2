function(set_project_warnings project_name)
    option(WARNINGS_AS_ERRORS "Treat compiler warnings as errors" TRUE)

    set(CLANG_WARNINGS
        -Wpedantic
        -Wdouble-promotion
        -Wnull-dereference
        -Wmissing-include-dirs
        -Wswitch-default
        -Wswitch-enum
        -Walloca
        -Wshadow
        -Wundef
        -Wunused-macros
        -Wbad-function-cast
        -Wcast-qual
        -Wcast-align
        -Wconversion
        -Wdangling-else
        -Wdate-time
        -Waggregate-return
        -Wstrict-prototypes
        -Wold-style-definition
        -Wmissing-prototypes
        -Wmissing-declarations
        -Wpacked
        -Wredundant-decls
        -Wnested-externs
        -Winline
        -Winvalid-pch
        -Wno-unused-parameter
    )

    set(GCC_WARNINGS
        ${CLANG_WARNINGS}
        -Wsync-nand
        -Walloc-zero
        -Wduplicated-branches
        -Wduplicated-cond
        -Wzero-length-bounds
        -Wjump-misses-init
        -Wlogical-op
        -Wno-unused-command-line-argument
    )

    if (WARNINGS_AS_ERRORS)
        set(CLANG_WARNINGS ${CLANG_WARNINGS} -Werror)
        set(GCC_WARNINGS ${GCC_WARNINGS} -Werror)
    endif ()

    if (CMAKE_C_COMPILER_ID MATCHES ".*Clang")
        set(PROJECT_WARNINGS ${CLANG_WARNINGS})
    elseif (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(PROJECT_WARNINGS ${GCC_WARNINGS})
    else ()
        message(AUTHOR_WARNING "No compiler warnings set for '${CMAKE_C_COMPILER_ID}' compiler.")
    endif ()

    target_compile_options(${project_name} INTERFACE ${PROJECT_WARNINGS})

endfunction()
