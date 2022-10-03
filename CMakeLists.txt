cmake_minimum_required(VERSION 3.20)

project(ir_builder)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/ir_builder.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ir/inst.cpp
)

add_compile_options(-std=c++1z -Wno-pointer-arith -g)

add_executable(ir_builder ${SOURCES})

target_include_directories(ir_builder PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/ir
)
