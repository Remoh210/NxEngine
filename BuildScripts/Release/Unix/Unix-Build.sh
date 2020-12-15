BUILT_TARGET="Release"

cd $(dirname "$0")
cd ../../

mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE="$BUILD_TARGET" ../
make
./NxEngine
