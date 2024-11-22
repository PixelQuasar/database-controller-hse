cmake_minimum_required(VERSION 3.26)

add_executable(CalculatorTests calculator_ut.cpp)

target_link_libraries(CalculatorTests PRIVATE Calculator gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(CalculatorTests)
