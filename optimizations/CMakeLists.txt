set(OPT_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/pass_manager.cpp
)

add_library(opts SHARED ${OPT_SOURCES})
target_include_directories(opts PRIVATE ${PROJECT_SOURCE_DIR})
