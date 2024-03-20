#!/usr/bin/env bash
echo $1 $2

conf=Release
if ! [ -f bin/$conf/aftests]; then
    rm -rf build
    cmake -B build -DCMAKE_BUILD_TYPE=$conf -G "Ninja Multi-Config"
    cmake --build build --config $conf
fi
cd bin/$conf
./aftests $1 $2 | tee -a af_results.txt
