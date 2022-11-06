cmake_minimum_required(VERSION 3.16)

project(compiler_opts)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(third-party/googletest)

add_compile_options(-Wno-return-type -g -Wno-narrowing)

add_subdirectory(ir)
add_subdirectory(pass)
add_subdirectory(tests)

add_executable(compiler_opts compiler_opts.cpp)

target_include_directories(compiler_opts PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(compiler_opts ir)
target_link_libraries(compiler_opts pass)
