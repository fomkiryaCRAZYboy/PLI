cmake_minimum_required(VERSION 3.20.0)
project(pli C)

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

file(GLOB SOURCES LIST_DIRECTORIES false src/*.c)

add_executable(pli ${SOURCES})
target_include_directories(pli PRIVATE include)

add_custom_target(fullclean
    COMMAND ${CMAKE_COMMAND} -E echo "Removing build configuration..."
    COMMAND ${CMAKE_COMMAND} -E remove -f ${CMAKE_SOURCE_DIR}/build/*
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_SOURCE_DIR}/build/CMakeFiles
)

message(STATUS "Parallel level: ${NUM_PROCESSORS}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")