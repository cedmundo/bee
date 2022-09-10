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
macro(setup_test TARGET TEST_SOURCES)
    add_executable(${TARGET} ${ARGN} ${TARGET}.c)
    target_link_libraries(${TARGET} PRIVATE cmocka-static)
    add_test(NAME ${TARGET} COMMAND ${TARGET})
endmacro()

# Testing
enable_testing()
setup_test(common_test common.h common.c)
setup_test(errors_test common.h common.c errors.h errors.c)
setup_test(dynamic_array_test common.h common.c errors.h errors.c dynamic_array.h dynamic_array.c)
setup_test(lexer_test common.h common.c errors.h errors.c lexer.h lexer.c)
setup_test(parser_lit_test common.h common.c errors.h errors.c dynamic_array.h dynamic_array.c lexer.h lexer.c parser.h parser.c)
setup_test(parser_primary_test common.h common.c errors.h errors.c dynamic_array.h dynamic_array.c lexer.h lexer.c parser.h parser.c)
