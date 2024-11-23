file(GLOB_RECURSE TEST_SOURCES "*.cpp")

add_executable(TableTests ${TEST_SOURCES})

target_link_libraries(TableTests PRIVATE Table gtest gtest_main)

include(GoogleTest)
gtest_discover_tests(TableTests)