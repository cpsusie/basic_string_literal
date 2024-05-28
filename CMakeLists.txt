cmake_minimum_required(VERSION 3.28)
project(string_literal)

set(CMAKE_CXX_STANDARD 20 REQUIRED)

add_library(bsl_lib)

target_sources(bsl_lib
    PUBLIC
        FILE_SET CXX_MODULES FILES
            basic_string_literal.cppm
)

add_executable(bsl_test main.cpp)
target_link_libraries(bsl_test PRIVATE bsl_lib)












