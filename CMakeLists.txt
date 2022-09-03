cmake_minimum_required(VERSION 3.16)
project(bee
        VERSION 0.0.1
        LANGUAGES C
        DESCRIPTION "Bee programming language prototype")

enable_language(C)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include scripts
include(cmake/FetchCMocka.cmake)

# Macros
macro(setup_test_target TARGET TEST_SOURCES)
    add_executable(${TARGET} ${ARGN} ${TARGET}.c)
    target_link_libraries(${TARGET} PRIVATE cmocka-static)
endmacro()

# Testing
enable_testing()
setup_test_target(common_test common.h common.c)
add_test(NAME common_test COMMAND common_test)