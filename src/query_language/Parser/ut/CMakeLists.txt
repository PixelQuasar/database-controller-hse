file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(ParserTests ${TEST_SOURCES})

target_link_libraries(ParserTests PRIVATE Parser gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(ParserTests)