cmake_minimum_required(VERSION 3.16)
project(bee VERSION 0.0.1 LANGUAGES C)

# Include scripts
include(cmake/FetchCMocka.cmake)
include(FindPkgConfig)
pkg_check_modules(LIBJIT libjit REQUIRED)

# Setup project variables
set(CMAKE_C_STANDARD 11)
add_definitions("-Wall" "-g")

set(BEE_SOURCES utf8.c)
set(BEE_HEADERS utf8.h)

# Main executable
add_executable(bee ${BEE_HEADERS} ${BEE_SOURCES} bee.c)
target_include_directories(bee PRIVATE ${LIBJIT_INCLUDE_DIRS})
target_link_libraries(bee PUBLIC ${LIBJIT_LIBRARIES})

# Testing
add_executable(utf8_test ${BEE_HEADERS} ${BEE_SOURCES} utf8_test.c)
target_compile_features(utf8_test PRIVATE c_std_99)
target_link_libraries(utf8_test PRIVATE cmocka-static)

enable_testing()
add_test(NAME utf8_test COMMAND utf8_test)