cmake_minimum_required(VERSION 3.26)
project(database_controller_hse)

set(CMAKE_CXX_STANDARD 20)

enable_testing()

add_subdirectory(external/googletest)
add_subdirectory(src/Calculator)
add_subdirectory(src/database/Database)
add_subdirectory(src/database/Table)
add_subdirectory(src/query_language/Result)
add_subdirectory(src/query_language/Executor)
add_subdirectory(src/query_language/Parser)
add_subdirectory(src/query_language/Query)

set(SOURCE_FILES
    main.cpp
    src/database/Database/Database.cpp
    src/database/Table/Table.cpp
    src/query_language/Executor/Executor.cpp
    src/query_language/Query/Query.cpp
        src/query_language/Result/Result.cpp
        src/types.cpp
)

add_executable(database ${SOURCE_FILES})

target_link_libraries(database PRIVATE Calculator Database Table Result Executor Parser Query)

target_compile_options(database PRIVATE
        $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
        $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>
)
