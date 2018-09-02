Screepsbot
==========

Naive screeps bot attempt

Building
--------

    mkdir build
    cd build
    cmake ../ -DCMAKE_TOOLCHAIN_FILE=/usr/lib/emscripten/cmake/Modules/Platform/Emscripten.cmake
    make

This should produce screepsbot.wasm