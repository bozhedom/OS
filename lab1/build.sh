#!/bin/bash

echo "Updating project from Git..."

git pull origin main

echo "Building project..."

mkdir -p build

cd build

cmake ..

make

echo "Build complete!"
