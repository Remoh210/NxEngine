@echo off
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release ../
mkdir Debug
mkdir Release

msbuild NxEngine.vcxproj /p:Configuration=Release
Release\NxEngine.exe

copy ..\lib\_bin\ Debug
copy ..\lib\_bin\ Release
cd ..
