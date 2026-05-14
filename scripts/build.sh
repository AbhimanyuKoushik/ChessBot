#!/bin/bash

# Making sure that script runs from the project root folder
cd "$(dirname "$0")/.."

# Make the build directory if it doesn't exist
mkdir -p build

# Go to the build directory
cd build

# Generates the make file as well as generates compile_commands.json
# We need compile_commands so the VS code plugin knows where headers 
#   and what version of cpp we are using
# -DCMAKE as and so on is required generating the compile_commands file
# Build type = release sets the compiler optimization properly (removing assert statements and stuff)
# Build type = debug sets off the compiler optimization and flags to -g and -O0 for debugginh
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release ..

# Compile the code using all the available cores
make -j$(nproc)

# Copy it to the project root folder so that VS code will know it
cp compile_commands.json ..

echo "Build Successful: run with ./build/bin/ChessEngine"