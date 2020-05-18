@echo off
mkdir build
cd build
cmake ../
mkdir Debug
mkdir Release
copy ..\lib\_bin\ Debug
copy ..\lib\_bin\ Release
cd ..
