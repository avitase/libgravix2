find_package(Doxygen REQUIRED dot)

set(DOXYGEN_GENERATE_HTML YES)
set(DOXYGEN_CALLER_GRAPH YES)
set(DOXYGEN_CALL_GRAPH YES)
set(DOXYGEN_EXTRACT_ALL YES)
set(DOXYGEN_GENERATE_LATEX YES)
set(DOXYGEN_USE_MATHJAX YES)
set(DOXYGEN_DISABLE_INDEX YES)
set(DOXYGEN_GENERATE_TREEVIEW YES)
set(DOXYGEN_USE_MDFILE_AS_MAINPAGE theory.md)
set(DOXYGEN_IMAGE_PATH ${PROJECT_SOURCE_DIR}/doc/img)
set(DOXYGEN_PROJECT_NAME "${PROJECT_NAME}")
set(DOXYGEN_PROJECT_NUMBER "${PROJECT_VERSION}")
set(DOXYGEN_PROJECT_BRIEF "${PROJECT_DESCRIPTION}")

doxygen_add_docs(doc ALL
    ${PROJECT_SOURCE_DIR}/doc/theory.md
    ${PROJECT_BINARY_DIR}/api
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
)
