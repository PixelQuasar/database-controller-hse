file(GLOB_RECURSE TEST_SOURCES "*.cpp")

# Создание исполняемого файла для тестов
add_executable(CalculatorTests ${TEST_SOURCES})

# Линковка тестового исполняемого файла с библиотекой Calculator и GoogleTest
target_link_libraries(CalculatorTests PRIVATE Calculator gtest gtest_main)

# Регистрация тестов с использованием GoogleTest
include(GoogleTest)
gtest_discover_tests(CalculatorTests)
