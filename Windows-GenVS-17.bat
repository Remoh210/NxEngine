@echo off
mkdir build
cd build
cmake -G "Visual Studio 15 2017" -A x64 -DCMAKE_BUILD_TYPE=Release ../
mkdir Debug
mkdir Release

copy ..\lib\_bin\ Debug
copy ..\lib\_bin\ Release
cd ..


// Copyright 2020 Offworld Industries


//#include "Player/TRPlayerAbility.h"
//
//UTRPlayerAbility::UTRPlayerAbility()
//	: Super()
//{
//}