cmake_minimum_required(VERSION 3.0.0)

project(6502 C)

include(CTest)
enable_testing()

set(CMAKE_C_STANDARD 11)
# Essential -Wall -Wextra
set(CMAKE_C_FLAGS_DEBUG_INIT "-Wall -Wextra -Werrer -Wshadow")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(src)

file(GLOB 6502_HEADER
    "${PROJECT_SOURCE_DIR}/src/h6502.h"
    "${PROJECT_SOURCE_DIR}/src/macros.h"
)
file(GLOB MAIN_SRC
    "${PROJECT_SOURCE_DIR}/src/*.c"
    "${PROJECT_SOURCE_DIR}/src/*.h"
)

add_executable(${CMAKE_PROJECT_NAME} ${MAIN_SRC})

#### UNITY TESING
# http://www.throwtheswitch.org/build/cmake
file(GLOB UNITY_SRC
    "${CMAKE_SOURCE_DIR}/tests/Unity/*.c"
    "${CMAKE_SOURCE_DIR}/tests/Unity/*.h"
)
file(GLOB TEST_SOURCES
    "${CMAKE_SOURCE_DIR}/tests/tests.c"
)

add_library(unity STATIC ${UNITY_SRC})
add_library(6502_header INTERFACE ${6502_HEADER})

add_executable(6502_tests ${TEST_SOURCES})
target_link_libraries(6502_tests ${6502_HEADER} unity)

set_target_properties(6502_tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/tests")

add_test(TESTING_6502 6502_tests) #add_test(TEST_NAME EXE_NAME_TO_TEST)