set(PASS_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/const_folding.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dom_tree_fast.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dom_tree_slow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/rpo.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dce.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loop_analyzer.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/peephole.cpp
)

add_library(pass SHARED ${PASS_SOURCES})
target_include_directories(pass PRIVATE ${PROJECT_SOURCE_DIR})
