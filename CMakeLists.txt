cmake_minimum_required(VERSION 3.16)
project(bee VERSION 0.0.1 LANGUAGES C)

# Include scripts
include(cmake/FetchCMocka.cmake)

macro(setup_test_target TARGET TEST_SOURCES)
    add_executable(${TARGET} ${ARGN} ${TARGET}.c)
    target_compile_features(${TARGET} PRIVATE c_std_99)
    target_link_libraries(${TARGET} PRIVATE cmocka-static)
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

enable_testing()
setup_test_target(parser_test lexer.h lexer.c parser.h parser.c)
add_test(NAME parser_test COMMAND parser_test)
