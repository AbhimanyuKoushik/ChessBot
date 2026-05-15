# Initializing the project
cmake_minimum_required(VERSION 3.15)
project(ChessEngine VERSION 0.1.0 LANGUAGES CXX)

# We need C++20 since we use constexpr and other bit stuff which is supported by C++20+
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF) 

# For getting compile_commands.json for VSCode cpp extension
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Adding optimization and warning flags
# If the user is compiling on windows then MSVC is on and the compilation flags change
if(MSVC)
    add_compile_options(/W4 /WX /O2)
else()
    add_compile_options(-Wall -Wextra -Werror -O3 -march=native)
endif()

# Search the src, tests folder and subfolders for the cpp files
file(GLOB_RECURSE SOURCES "src/*.cpp")

# Keeping the binary file in build/bin folder instead of build folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Creating the final executable
add_executable(ChessEngine apps/main.cpp ${SOURCES})

# To include all the header files
target_include_directories(ChessEngine PUBLIC include)

# Enabling cmake's internal testing
enable_testing()

# Find all the cpp files in the tests directory
file(GLOB_RECURSE TEST_SOURCES "tests/*.cpp")

# We will create a separate executable for testing called test_main.cpp
add_executable(EngineTests ${SOURCES} ${TEST_SOURCES})

# Tell test excutable where the headers are
target_include_directories(EngineTests PUBLIC include)

# Register the executable with Ctest
add_test(NAME "Bitboard_Test" COMMAND EngineTests bitboard)