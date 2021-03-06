cmake_minimum_required (VERSION 2.8)
project (hm)
include(CheckCXXCompilerFlag)

function(enable_cxx_compiler_flag_if_supported flag)
    string(FIND "${CMAKE_CXX_FLAGS}" "${flag}" flag_already_set)
    if(flag_already_set EQUAL -1)
        check_cxx_compiler_flag("${flag}" flag_supported)
        if(flag_supported)
            set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${flag}" PARENT_SCOPE)
        endif()
        unset(flag_supported CACHE)
    endif()
endfunction()

enable_cxx_compiler_flag_if_supported("-Wall")
enable_cxx_compiler_flag_if_supported("-Wextra")
enable_cxx_compiler_flag_if_supported("-pedantic")
enable_cxx_compiler_flag_if_supported("-std=c++17")

file(GLOB SOURCES *.h *.cc library/*.h library/*.cc library/pointers/*.h library/pointers/*.cc)

set(TestH "${CMAKE_CURRENT_SOURCE_DIR}/test.h")
add_executable(${PROJECT_NAME} ${SOURCES} ${TestH})

add_custom_command(
    COMMAND ./combine_all_tests.py
    DEPENDS combine_all_tests.py
    OUTPUT ${TestH}
    COMMENT "Generating code for test.h"
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
)

