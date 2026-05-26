@echo off
if exist build rmdir /s /q build
mkdir build
cd build
cmake -G Ninja -DCMAKE_MAKE_PROGRAM=ninja ..
