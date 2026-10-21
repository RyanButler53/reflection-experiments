cmake_minimum_required(VERSION 4.0)
project(reflection)

# hard code compiler since default compiler doesn't support this
set(CMAKE_C_COMPILER "/opt/homebrew/bin/gcc-16")
set(CMAKE_CXX_COMPILER "/opt/homebrew/bin/g++-16")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options("-freflection")


# don't do this!
include_directories(inc)

add_subdirectory(src)
