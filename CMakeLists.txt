
cmake_minimum_required(VERSION 3.10)

project(6502 C)

set(CMAKE_C_STANDARD 11)
# Essential -Wall -Wextra
set(CMAKE_C_FLAGS_DEBUG_INIT "-Wall -Wextra -Werrer -Wshadow")
#set(CMAKE_C_FLAGS_RELEASE_INIT        "-O3 -DNDEBUG")


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(
    ${PROJECT_SOURCE_DIR}/src
)
file(GLOB all_SRCS
    "${PROJECT_SOURCE_DIR}/src/*.c"
)

add_executable(${CMAKE_PROJECT_NAME} ${all_SRCS})