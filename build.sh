#!/bin/bash

BUILD_TYPE=${1:-Debug}

cmake \
    -DCMAKE_BUILD_TYPE=${BUILD_TYPE} \
    -DCMAKE_MAKE_PROGRAM=ninja \
    -B ./build \
    -G Ninja \
    -S ./

# do ninja things
cd ./build || exit 1 # exit if build directory doesn't exist
ninja

echo "📦 Build complete"