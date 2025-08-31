@echo off
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ../
mkdir Debug
mkdir Release

copy ..\lib\_bin\ Debug
copy ..\lib\_bin\ Release
cd ..