cmake_minimum_required(VERSION 3.16)
project(bee)

# Find libraries
include(FindPkgConfig)
pkg_check_modules(LIBJIT libjit REQUIRED)

# Setup project variables
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

add_definitions("-Wall" "-g")
set(PROJECT_LIBNAME bji)
set(LIB_INCLUDE include)
set(BIN_SOURCES src/main.c)
file(GLOB LIB_HEADERS src/*.h)
file(GLOB LIB_SOURCES src/*.c)
list(REMOVE_ITEM LIBS_SOURCES ${BIN_SOURCES})

# Add bee library
add_library(${PROJECT_LIBNAME} ${LIB_HEADERS} ${LIB_SOURCES})
target_include_directories(${PROJECT_LIBNAME} PUBLIC ${LIB_INCLUDE})
target_include_directories(${PROJECT_LIBNAME} PRIVATE ${LIBJIT_INCLUDE_DIRS})

# Add bee executable
add_executable(${PROJECT_NAME} src/main.c)
target_include_directories(${PROJECT_NAME} PUBLIC ${LIB_INCLUDE})
target_include_directories(${PROJECT_NAME} PRIVATE ${LIBJIT_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC ${PROJECT_LIBNAME} ${LIBJIT_LIBRARIES})

# Add bee tests
add_subdirectory(vendor/googletest)
set(TEST_BINARY ${PROJECT_LIBNAME}_test)
set(GTEST_LIBRARY gtest)
file(GLOB TEST_SOURCES test/*.cpp)
file(GLOB TEST_HEADERS test/*.h)

add_executable(${TEST_BINARY} ${TEST_HEADERS} ${TEST_SOURCES})
target_include_directories(${TEST_BINARY} PUBLIC ${LIB_INCLUDE})
target_include_directories(${TEST_BINARY} PRIVATE ${LIBJIT_INCLUDE_DIRS})
target_link_libraries(${TEST_BINARY} PUBLIC ${PROJECT_LIBNAME} ${LIBJIT_LIBRARIES} ${GTEST_LIBRARY})

add_test(NAME ${TEST_BINARY} COMMAND ${TEST_BINARY})