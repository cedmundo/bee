cmake_minimum_required(VERSION 3.16)
project(bee VERSION 0.0.1 LANGUAGES C)

# Include scripts
include(cmake/FetchCMocka.cmake)

# For libjit
find_package(PkgConfig)
pkg_check_modules(LIBJIT REQUIRED libjit)

macro(setup_test_target TARGET TEST_SOURCES)
    add_executable(${TARGET} ${ARGN} ${TARGET}.c)
    target_compile_options(${TARGET} PUBLIC ${LIBJIT_CFLAGS_OTHER})
    target_compile_features(${TARGET} PRIVATE c_std_99)
    target_include_directories(${TARGET} PUBLIC ${LIBJIT_INCLUDE_DIRS})
    target_link_libraries(${TARGET} PRIVATE cmocka-static ${LIBJIT_LIBRARIES})
endmacro()

# Setup project variables
set(CMAKE_C_STANDARD 17)
add_definitions("-Wall" "-g")

set(BEE_HEADERS)
set(BEE_SOURCES)

# Main executable
# add_executable(bee ${BEE_HEADERS} ${BEE_SOURCES} bee.c)
# target_include_directories(bee PRIVATE ${LIBJIT_INCLUDE_DIRS})
# target_link_libraries(bee PUBLIC ${LIBJIT_LIBRARIES})

# Testing
enable_testing()
setup_test_target(lexer_test lexer.h lexer.c)
add_test(NAME lexer_test COMMAND lexer_test)

setup_test_target(parser_test lexer.h lexer.c parser.h parser.c)
add_test(NAME parser_test COMMAND parser_test)

setup_test_target(compiler_test lexer.h lexer.c parser.h parser.c compiler.h compiler.c)
add_test(NAME compiler_test COMMAND compiler_test)

setup_test_target(scope_test lexer.h lexer.c parser.h parser.c compiler.h compiler.c scope.h scope.c)
add_test(NAME scope_test COMMAND scope_test)