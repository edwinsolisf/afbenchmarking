#!/usr/bin/env bash

if [ "$#" -ne 1 ]; then
    echo "Missing path to benchdnn"
    exit 1
fi

dir=$(pwd)

engine=cpu
iter=20
reps=1000

cd $1
./benchdnn --matmul --engine=$engine --fast-ref=true --mem-check=false --mode=f \
           --mode-modifier=p --repeats-per-prb=$reps --fix-times-per-prb=$iter \
           --perf-template=%driver%,%engine%,%-time%,%0time%,%+time%,%0ctime% --stag=abcd \
           --wtag=abcd --dtag=abcd --attr-scratchpad=user 1x8x1500x1500:1x8x1500x64 > $dir/matmul_results.txt

./benchdnn --conv --engine=$engine --fast-ref=true --mem-check=false --mode=f \
           --mode-modifier=p --repeats-per-prb=$reps --fix-times-per-prb=$iter \
           --perf-template=%driver%,%engine%,%-time%,%0time%,%+time%,%0ctime% --stag=aBc8b \
           --wtag=ABc8b8a --dtag=aBc8b --attr-scratchpad=user --attr-post-ops=eltwise_gelu_erf \
           --dir=FWD_I \
           mb1_ic512oc512_iw3000ow1500kw3sw2dw0pw1 > $dir/conv_results.txt

./benchdnn --softmax --engine=$engine --fast-ref=true --mem-check=false --mode=f \
           --mode-modifier=p --repeats-per-prb=$reps --fix-times-per-prb=$iter \
           --perf-template=%driver%,%engine%,%-time%,%0time%,%+time%,%0ctime% --stag=abcd \
           --stag=abcd --attr-scratchpad=user --axis=3 1x8x1500x1500 > $dir/softmax_results.txt
       
cd $dir

python parse_benchdnn.py matmul_results.txt conv_results.txt softmax_results.txt

