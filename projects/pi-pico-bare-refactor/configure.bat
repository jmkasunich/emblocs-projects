@echo off
if exist build rmdir /s /q build
mkdir build
python ..\..\submodules\emblocs\python\variant_compiler.py blink.blocs build\
cd build
cmake -G Ninja -DCMAKE_MAKE_PROGRAM=ninja ..
