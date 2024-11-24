file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(CalculatorTests ${TEST_SOURCES})

target_link_libraries(CalculatorTests PRIVATE Calculator gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(CalculatorTests)
