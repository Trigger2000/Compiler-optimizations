cmake_minimum_required(VERSION 3.20)

project(ir_builder)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/compiler_opt.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inst.cpp
)

add_compile_options(-std=c++1z -Wno-pointer-arith -g)

add_executable(ir_builder ${SOURCES})

target_include_directories(ir_builder PUBLIC
    "${PROJECT_BINARY_DIR}"
)
