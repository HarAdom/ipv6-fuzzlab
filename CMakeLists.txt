cmake_minimum_required(VERSION 3.16)

project(ipv6-fuzzlab
    VERSION 0.1.0
    DESCRIPTION "IPv6 protocol robustness and security research toolkit"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_compile_options(
    -Wall
    -Wextra
    -Wpedantic
    -Wconversion
    -Wshadow
)

add_library(ipv6fuzzlab_core
    src/parser.c
)

target_include_directories(ipv6fuzzlab_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_executable(ipv6-fuzzlab
    src/main.c
)

target_link_libraries(ipv6-fuzzlab PRIVATE ipv6fuzzlab_core)

enable_testing()

add_executable(test_parser
    tests/test_parser.c
)

target_link_libraries(test_parser PRIVATE ipv6fuzzlab_core)

add_test(NAME parser_regression COMMAND test_parser)
