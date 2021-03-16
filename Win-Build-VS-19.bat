@echo off
mkdir build
cd build
cmake -G "Visual Studio 16 2019" -A win32 -DCMAKE_BUILD_TYPE=Release ../
mkdir Debug
mkdir Release

msbuild NxEngine.vcxproj /p:Configuration=Release
Release\NxEngine.exe

copy ..\lib\_bin\ Debug
copy ..\lib\_bin\ Release
cd ..
pause