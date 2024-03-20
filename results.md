Results
===========

Tests: 1000 repetitions of 20 iterations
Units : $\text{ms}$

ArrayFire v3.9.0 (OpenCL/OneApi, 64-bit Linux, build 4061db86e)
On Intel(R) Level-Zero: Intel(R) Arc(TM) A770 Graphics, 15473 MB (fp16)
|   Test   |           ArrayFire OneApi            |            ArrayFire OpenCL              |        OneDNN                          |
|:--------:|:-------------------------------------:|:----------------------------------------:|:--------------------------------------:|
|  matmul  |             $0.399\pm0.012$           |               $3.34\pm0.05$              |  $${\color{lightgreen}0.377\pm0.22}$$  |
|  conv    |             $1.61\pm0.10$             | $${\color{lightgreen}{1.14\pm0.01}}$$    |            $2.06\pm0.32$               |
|  softmax |             $6.91\pm0.07$             | $${\color{lightgreen}{1.28\pm0.02}}$$    |            $2.01\pm0.50$               |
|  inner   | $${\color{lightgreen}0.616\pm0.055}$$ |             $0.828\pm0.019$              |            $5.81\pm 0.10$              |