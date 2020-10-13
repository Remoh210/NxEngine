@echo off
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A x64 ../
mkdir Debug
mkdir Release

copy ..\lib\_bin\ Debug
copy ..\lib\_bin\ Release
cd ..
