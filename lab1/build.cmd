@echo off

echo Updating project from Git...

git pull origin master

if %ERRORLEVEL% neq 0 (
    echo Error: Failed to update project from Git.
    exit /b 1
)

echo Building project...

if not exist build (
    echo "Build directory not found. Creating build directory..."
    mkdir build
) else (
    echo "Build directory already exists."
)

cd build

cmake -G "MinGW Makefiles" ..

if %ERRORLEVEL% neq 0 (
    echo Error: CMake configuration failed.
    exit /b 1
)

cmake --build .

if %ERRORLEVEL% neq 0 (
    echo Error: Build failed.
    exit /b 1
)

echo Build complete!
