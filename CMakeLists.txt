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

##
## TESTING
##

file(GLOB TEST_FILES
    "${CMAKE_SOURCE_DIR}/tests/*.c"
)
set(TEST_NAMES_LIST 
    "LDA_TESTS" 
    "LDX_TESTS"
)

message(STATUS "[TESTS] Generating all add_test...")

set(i 0)
foreach(file ${TEST_FILES})
    # Get test name "test_name"
    list(GET TEST_NAMES_LIST ${i} test_name) # TEST_NAMES_LIST doesnt need to be in ${}

    # Create .exe
    add_executable(${test_name} ${file})
    # Link the 6502 and Unity headers
    target_link_libraries(${test_name} 6502_header unity)
    # Set output folder
    set_target_properties(${test_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/tests")
    # Add the test for Ctest, 
    add_test(6502_${test_name} "${CMAKE_SOURCE_DIR}/bin/tests/${test_name}")

    message(STATUS "[TESTS] ${i} - ${file} with test name : 6502_${test_name}")
    math(EXPR i "${i} + 1")
endforeach()
