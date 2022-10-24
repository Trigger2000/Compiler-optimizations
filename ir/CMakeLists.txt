set(IR_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/inst.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/basic_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/utils.cpp
)

add_library(ir SHARED ${IR_SOURCES})
target_include_directories(ir PRIVATE ${PROJECT_SOURCE_DIR})
