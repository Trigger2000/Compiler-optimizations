cmake_minimum_required(VERSION 3.16)

project(ir_builder)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_builder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/inst.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/basic_block.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/utils.cpp
)

add_compile_options(-std=c++1z -Wno-return-type -g -Wno-narrowing)

add_executable(ir_builder ${SOURCES})

target_include_directories(ir_builder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
