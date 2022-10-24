set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/graph_test.cpp
)

set(GTEST_INCLUDE_DIR third-party/googletest/googletest/include)

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests ir gtest pthread)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR} SHARED ${GTEST_INCLUDE_DIR})
