#!/bin/bash

echo "Updating project from Git..."

git pull origin master

if [ $? -ne 0 ]; then
    echo "Error: Failed to update project from Git."
    exit 1
fi

echo "Building project..."

mkdir -p build

cd build

cmake ..

if [ $? -ne 0 ]; then
    echo "Error: CMake configuration failed."
    exit 1
fi

make

if [ $? -ne 0 ]; then
    echo "Error: Build failed."
    exit 1
fi

echo "Build complete!"