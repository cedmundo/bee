cmake_minimum_required(VERSION 3.16)
project(bee)

# Find libraries
include(FindPkgConfig)
pkg_check_modules(LIBJIT libjit REQUIRED)

# Setup project variables
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 14)

add_executable(bee bee.c)
