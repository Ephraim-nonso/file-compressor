cmake_minimum_required(VERSION 3.16)
project(File-Compression-System)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(compression
    src/RLECompression.cpp
    src/FileHandler.cpp
    src/Server.cpp
    src/Client.cpp
    src/IdentityCompression.cpp
    src/AdaptiveCompression.cpp
)
target_include_directories(compression PUBLIC include) # Changed from src to include
target_link_libraries(compression PUBLIC pthread)

add_executable(server src/ServerMain.cpp)
target_link_libraries(server PRIVATE compression pthread)
target_include_directories(server PRIVATE include) # Add include directory for server

add_executable(client src/ClientMain.cpp)
target_link_libraries(client PRIVATE compression pthread)
target_include_directories(client PRIVATE include) # Add include directory for client

include(FetchContent)
FetchContent_Declare(
  catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG v3.5.0
)
FetchContent_MakeAvailable(catch2)

include(CTest) # Ensure CTest is included before adding tests

file(GLOB TEST_SOURCES tests/*.cpp)
message(STATUS "DEBUG: Found test sources: ${TEST_SOURCES}") # Added debug message

foreach(TEST_SOURCE_FILE ${TEST_SOURCES})
    # Extract the base name of the test file (e.g., "test1_rle_roundtrip" from "tests/test1_rle_roundtrip.cpp")
    get_filename_component(TEST_NAME ${TEST_SOURCE_FILE} NAME_WE)

    # Create an executable for each test file
    add_executable(${TEST_NAME} ${TEST_SOURCE_FILE})

    # Link necessary libraries to the individual test executable
    target_link_libraries(${TEST_NAME} PRIVATE compression Catch2::Catch2WithMain pthread)
    target_include_directories(${TEST_NAME} PRIVATE include) # Add include

    # Add a CTest test for each executable
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})
    message(STATUS "DEBUG: Added test executable '${TEST_NAME}' from source '${TEST_SOURCE_FILE}'") # Added debug message
endforeach()
