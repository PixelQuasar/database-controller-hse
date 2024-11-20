find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(CalculatorTests test.cpp)

target_link_libraries(CalculatorTests PRIVATE Calculator GTest::GTest GTest::Main)

add_test(NAME CalculatorTests COMMAND CalculatorTests)
