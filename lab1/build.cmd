@echo off

echo Updating project from Git...

git pull origin master

echo Building project...

if not exist build (
    echo "Build directory not found. Creating build directory..."
    mkdir build
) else (
    echo "Build directory already exists."
)

cd build

cmake -G "MinGW Makefiles" ..

cmake --build .

echo Build complete!
