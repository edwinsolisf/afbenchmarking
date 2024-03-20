Results
===========

Tests: 1000 repetitions of 20 iterations
Units : $\text{ms}$

ArrayFire v3.9.0 (OpenCL/OneApi, 64-bit Linux, build 4061db86e)
On Intel(R) Level-Zero: Intel(R) Arc(TM) A770 Graphics, 15473 MB (fp16)
|   Test   | ArrayFire OneApi | ArrayFire OpenCL     |        OneDNN        |
|:--------:|:----------------:|:--------------------:|:--------------------:|
|  matmul  |  $0.399\pm0.012$ | $3.34\pm0.05$        |  **$0.377\pm0.22$**  |
|  conv    |  $1.61\pm0.10$   | **$1.14\pm0.01$**    |   $2.06\pm0.32$      |
|  softmax |  $6.91\pm0.07$   | **$1.28\pm0.02$**    |   $2.01\pm0.50$      |