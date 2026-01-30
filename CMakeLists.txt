# usage

# cd build
# cmake ..

# build pli: cmake --build . --target pli
# parallel build: cmake --build . --target pli --parallel
# verbose output: cmake --build . --target pli -v 

# remove pli-binary: cmake --build . --target clean
# remove build configuration: cmake --build . --target fullclean


# debug build:

# cmake .. -DCMAKE_BUILD_TYPE=Debug 
# cmake --build . --target pli -v 


# file input flag controle:

# cmake --build . --target fullclean
# cmake .. -DFILE_INPUT=ON  /  cmake .. -DFILE_INPUT=OFF   (default is OFF)
# cmake --build . --target pli -v


# static analyze:

# cmake ..
# cmake --build . --target analyze > tidy.log


# preprocess (view files after preprocessing):

# cmake ..
# cmake --build . --target preprocess
# cat preprocessed/err_preprocessed.c

cmake_minimum_required(VERSION 3.20.0)
project(pli C)

# Enable compile_commands.json generation for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(ProcessorCount)
ProcessorCount(NUM_PROCESSORS)

if(NUM_PROCESSORS EQUAL 0)
    set(NUM_PROCESSORS 4)
endif()

set(CMAKE_C_FLAGS_RELEASE "-O2" CACHE STRING "Flags for Release build" FORCE)
set(CMAKE_C_FLAGS_DEBUG "-O0 -g" CACHE STRING "Flags for Debug build" FORCE)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(FILE_INPUT "Enable file input mode" OFF)

if(FILE_INPUT)
    add_compile_definitions(FILE_INPUT)
    message(STATUS "File input mode: ENABLED")
else()
    message(STATUS "File input mode: DISABLED")
endif()

set(TIDY_CHECKS
    "performance-*"
    "cert*"
    "bugprone*"
    "clang-analyzer*"
    "-clang-analyzer-security.insecureAPI.DeprecatedOrUnsafeBufferHandling")
    
string(REPLACE ";" "," TIDY_CHECKS_STR "${TIDY_CHECKS}")

file(GLOB SOURCES LIST_DIRECTORIES false src/*.c)

add_executable(pli ${SOURCES})
target_include_directories(pli PRIVATE include)

add_custom_target(fullclean
    COMMAND ${CMAKE_COMMAND} -E echo "Removing build configuration..."
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/build/*
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/preprocessed
)

add_custom_target(analyze
    COMMAND clang-tidy -p=${CMAKE_BINARY_DIR} -checks=${TIDY_CHECKS_STR} ${SOURCES}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Running clang-tidy on pli sources..."
)

add_custom_target(preprocess
    COMMAND ${CMAKE_COMMAND} -E echo "Preprocessing all source files..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/preprocessed
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Preprocessing source files..."
)

foreach(SOURCE_FILE ${SOURCES})
    get_filename_component(FILE_NAME ${SOURCE_FILE} NAME_WE)
    add_custom_command(TARGET preprocess POST_BUILD
        COMMAND ${CMAKE_C_COMPILER} -E -I${CMAKE_SOURCE_DIR}/include 
                $<$<BOOL:${FILE_INPUT}>:-DFILE_INPUT>
                ${SOURCE_FILE} -o ${CMAKE_BINARY_DIR}/preprocessed/${FILE_NAME}_preprocessed.c
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Preprocessing ${FILE_NAME}.c..."
    )
endforeach()

message(STATUS "Parallel level: ${NUM_PROCESSORS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")