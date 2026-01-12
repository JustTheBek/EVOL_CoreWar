#!/bin/bash
# build_galib_full.sh
# Full GALib build and tester script for macOS


# Stop immediately if any command fails
set -e

#################################################################
echo "==> Building GALib library..."

# Navigate to the ga/ directory
cd ga || { echo "ga/ directory not found!"; exit 1; }

# Build using make with C++98 (ensures compatibility)
make clean
make CXX="g++ -std=c++98"
#make CXX="g++ -std=c++17". ATTENTION: this won't work galib source is too old

# Check if the library was created
if [ ! -f libga.a ]; then
    echo "Error: libga.a was not created!"
    exit 1
fi

echo "GALib library built successfully!"
cd ..

#################################################################
echo "==> Building galib_tester..."


# Compile galib_tester.cpp linking with GALib
g++ -std=c++98 -I. -L./ga galib_tester.cpp -lga -o galib_tester

# Check if executable was created
if [ ! -f galib_tester ]; then
    echo "Error: galib_tester executable was not created!"
    exit 1
fi

echo "==> Running galib_tester..."
./galib_tester

echo "==> Done!"
