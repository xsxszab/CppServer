cmake_minimum_required(VERSION 3.4.1)

project(cppserver
        VERSION 0.1
        DESCRIPTION "simple cpp webserver"
        LANGUAGES C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 14) 
set(CMAKE_CXX_STANDARD_REQUIRED True) 

set(CMAKE_CXX_FLAGS "-pthread")

# do not run cmake in project root directory, run in ./build/ instead
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" PATH_TO_CMAKELISTS_TXT)
if (EXISTS "${PATH_TO_CMAKELISTS_TXT}")
    message(FATAL_ERROR "Run CMake from a build subdirectory! \"mkdir build ; cd build ; cmake .. \" \
    Some junk files were created in this folder (CMakeCache.txt, CMakeFiles); you should delete those.")
endif ()

set(CPPSERVER_SCRIPTS_DIR ${CMAKE_SOURCE_DIR}/scripts)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wall -std=c++14 -pthread")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -ggdb -fno-omit-frame-pointer -fno-optimize-sibling-calls")
# set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=address")
set(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -fPIC")
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fPIC")
set(CMAKE_STATIC_LINKER_FLAGS "${CMAKE_STATIC_LINKER_FLAGS} -fPIC")

message(STATUS "CMAKE_CXX_FLAGS: " ${CMAKE_CXX_FLAGS})
message(STATUS "CMAKE_CXX_FLAGS_DEBUG: " ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "CMAKE_EXE_LINKER_FLAGS: " ${CMAKE_EXE_LINKER_FLAGS})
message(STATUS "CMAKE_SHARED_LINKER_FLAGS: " ${CMAKE_SHARED_LINKER_FLAGS})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SEARCH_PATH "/usr/local/bin" "/usr/bin" "/usr/local/opt/llvm/bin")

# *****************************
# clang-format configuration
# format all source files: make format
# check if formatting is needed: make check-format
# *****************************

if (NOT DEFINED CLANG_FORMAT_BIN)
    find_program(CLANG_FORMAT_BIN
        NAMES clang-format clang-format-8
        HINTS ${SEARCH_PATH})
endif()

if ("${CLANG_FORMAT_BIN}" STREQUAL "CLANG_FORMAT_BIN-NOTFOUND")
    message(WARNING "cannot found clang-format executable")
else ()
    message(STATUS "found clang-format at " ${CLANG_FORMAT_BIN})
endif()

string(CONCAT FORMAT_DIRS
        "${CMAKE_SOURCE_DIR}/src,"
        "${CMAKE_SOURCE_DIR}/test,")

add_custom_target(format ${CMAKE_SOURCE_DIR}/scripts/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${CMAKE_SOURCE_DIR}/scripts/clang_format_exclusions.txt
        --source_dirs
        ${FORMAT_DIRS}
        --fix
        --quiet
        )

add_custom_target(check-format ${CMAKE_SOURCE_DIR}/scripts/run_clang_format.py
        ${CLANG_FORMAT_BIN}
        ${CMAKE_SOURCE_DIR}/scripts/clang_format_exclusions.txt
        --source_dirs
        ${FORMAT_DIRS}
        --quiet
        )

# *****************************
# clang-tidy configuration
# *****************************

if (NOT DEFINED CLANG_TIDY_BIN)
    # attempt to find the binary if user did not specify
    find_program(CLANG_TIDY_BIN
            NAMES clang-tidy clang-fidy-8
            HINTS ${PINE_CLANG_SEARCH_PATH})
endif ()
if ("${SEARCH_PATH}" STREQUAL "CLANG_TIDY_BIN-NOTFOUND")
    message(WARNING "cannot find clang-tidy executable")
else ()
    # Output compile_commands.json
    message(STATUS "Pine/main found clang-fidy at ${CLANG_TIDY_BIN}")
endif ()

add_custom_target(clang-tidy ${CMAKE_SOURCE_DIR}/scripts/run_clang_tidy.py
        -clang-tidy-binary ${CLANG_TIDY_BIN}
        -p ${CMAKE_BINARY_DIR})


# *****************************
# cpplint configuration
# *****************************

# TODO: add cpplint command


# *****************************


include_directories(${CMAKE_SOURCE_DIR}/src/include)


add_subdirectory(./src)
add_subdirectory(./test)