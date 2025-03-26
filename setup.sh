#!/bin/bash

# Step 1: Delete the build folder if it exists
if [ -d "build" ]; then
    echo "Deleting existing build directory..."
    rm -rf build
fi

# Step 2: Create a new build directory and navigate into it
echo "Creating new build directory..."
mkdir build
cd build

# Step 3: Run CMake to generate the Makefile
echo "Running CMake to generate Makefile..."
cmake ..

# Step 4: Run make to build the project
echo "Building the project..."
make

# Step 5: Return to the original directory
cd ..

echo "Build process completed."