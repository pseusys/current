rem @echo off

cd ../logic

cmake -G "MinGW Makefiles" .
mingw32-make LogicDll

rmdir /s /q .\CMakeFiles
del /f /q .\cmake_install.cmake
del /f /q .\CMakeCache.txt
del /f /q .\libLogicDll.dll.a
del /f /q .\Makefile

copy /y libLogicDll.dll ..\interface\Assets\LogicDll.dll
del /f /q .\libLogicDll.dll

pause