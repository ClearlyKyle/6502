cmake_minimum_required(VERSION 3.11)

project(6502 C)

include(CTest)
enable_testing()

if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
set (CMAKE_C_FLAGS "-std=gnu11 ${CMAKE_C_FLAGS}")
endif ()

# Essential -Wall -Wextra
add_compile_options(-Wall -Wextra -pedantic -Werror)

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
add_library(unity STATIC ${UNITY_SRC})
add_library(6502_header INTERFACE ${6502_HEADER})

##
## TESTING
##

#set(TEST_FILES
#    "${CMAKE_SOURCE_DIR}/tests/Load_Operation_tests.c"
#    "${CMAKE_SOURCE_DIR}/tests/Store_Operation_tests.c"
#    "${CMAKE_SOURCE_DIR}/tests/Jumps_and_Calls_tests.c"
#)

# These need to match the test file name, and will be make into a 
# test name of "6502_${name}"
set(TEST_NAMES_LIST 
    "Load_Operation_tests"
    "Store_Operation_tests" 
    "Jumps_and_Calls_tests"
    "Stack_Operation_tests"
    "AND_EOR_ORA_tests"
    "Load_Program_tests"
    "Transfer_Register_tests"
)

message(STATUS "[TESTS] Loading all test files...")

set(i 0)
foreach(name ${TEST_NAMES_LIST})
    message(STATUS "[TESTS] ${i} - ${CMAKE_SOURCE_DIR}/tests/${name}.c \t: 6502_${name}")

    # Create .exe
    add_executable(${name} "${CMAKE_SOURCE_DIR}/tests/${name}.c")
    # Link the 6502 and Unity headers
    target_link_libraries(${name} 6502_header unity)
    # Set output folder
    set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin/tests")
    # Add the test for Ctest, 
    add_test(6502_${name} "${CMAKE_SOURCE_DIR}/bin/tests/${name}")

    math(EXPR i "${i} + 1")
endforeach()

# will build before CTest is ran
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND}
                  DEPENDS ${TEST_NAMES_LIST})
