#!/usr/bin/env bash

mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build . --config RelWithDebInfo
cd ../bin/RelWithDebInfo
./afMatmulTest | tee af_results.txt
