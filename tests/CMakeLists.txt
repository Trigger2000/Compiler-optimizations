set(TEST_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/tests.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/dominator_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/ir_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/loop_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/const_folding_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/peephole_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/inline_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/check_elimination_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/linear_order_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/liveness_test.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/reg_alloc_test.cpp
)

set(GTEST_INCLUDE_DIR third-party/googletest/googletest/include)

add_executable(tests ${TEST_SOURCES})
target_link_libraries(tests ir pass gtest pthread)
target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR} SHARED ${GTEST_INCLUDE_DIR})
