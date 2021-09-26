cmake_minimum_required(VERSION 3.16)
project(bee C)

# Find libraries
include(FindPkgConfig)
pkg_check_modules(LIBJIT libjit REQUIRED)

# Setup project variables
set(CMAKE_C_STANDARD 11)
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
target_link_libraries(${PROJECT_NAME} ${LIBJIT_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${PROJECT_LIBNAME})