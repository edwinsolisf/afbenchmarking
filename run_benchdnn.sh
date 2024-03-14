#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
    echo "Missing path to benchdnn"
    exit 1
fi

dir=$(pwd)

cd $1
./benchdnn --matmul --engine=cpu --fast-ref=true --mem-check=false --mode=f \
           --mode-modifier=p --repeats-per-prb=1000 --fix-times-per-prb=20 \
           --perf-template=%driver%,%engine%,%-time%,%0time%,%+time%,%0ctime% --stag=abcd \
           --wtag=abcd --dtag=abcd --attr-scratchpad=user 1x8x1500x1500:1x8x1500x64 > $dir/benchdnn_results.txt
cd $dir

python parse_benchdnn.py

