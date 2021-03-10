#!/bin/sh
BUILT_TARGET="Release"

cd $(dirname "$0")
mkdir build
cd build
cmake  -fPIC -DCMAKE_C_COMPILER="clang" -DCMAKE_BUILD_TYPE="$BUILD_TARGET" ../
make
./NxEngine
