file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(LoggerTests ${TEST_SOURCES})

target_link_libraries(LoggerTests PRIVATE Logger gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(LoggerTests)