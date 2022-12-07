@echo off
mkdir build
cd build
cmake -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release ../
mkdir Debug
mkdir Release

msbuild NxEngine.vcxproj /p:Configuration=Release
Release\NxEngine.exe

copy ..\lib\_bin\ Debug
copy ..\lib\_bin\ Release
cd ..
pause