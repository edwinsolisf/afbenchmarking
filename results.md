Results
===========

Tests: 1000 repetitions of 20 iterations
Units : $\text{ms}$

ArrayFire v3.9.0 (OpenCL/oneAPI, 64-bit Linux, build 4061db86e)
On Intel(R) Level-Zero: Intel(R) Arc(TM) A770 Graphics, 15473 MB (fp16)
|   Test   |           ArrayFire oneAPI            |            ArrayFire OpenCL              |        OneDNN                          |
|:--------:|:-------------------------------------:|:----------------------------------------:|:--------------------------------------:|
|  matmul  |             $0.399\pm0.012$           |               $3.34\pm0.05$              |  $${\color{lightgreen}0.377\pm0.22}$$  |
|  conv    |             $1.61\pm0.10$             | $${\color{lightgreen}{1.14\pm0.01}}$$    |            $2.06\pm0.32$               |
|  softmax |             $6.91\pm0.07$             | $${\color{lightgreen}{1.28\pm0.02}}$$    |            $2.01\pm0.50$               |
|  inner   | $${\color{lightgreen}0.616\pm0.055}$$ |             $0.828\pm0.019$              |            $5.81\pm 0.10$              |

ArrayFire v3.9.0 (CUDA, 64-bit Linux, build 4061db86e)
Platform: CUDA Runtime 12.3, Driver: 545.29.06
[0] Tesla T4, 14930 MB, CUDA Compute 7.5
|   Test   |           ArrayFire Cuda              |            ArrayFire OpenCL              | OneDNN (relu instead of gelu_erf)      |
|:--------:|:-------------------------------------:|:----------------------------------------:|:--------------------------------------:|
|  matmul  |$${\color{lightgreen}0.600\pm0.039}$$  |               $2.25\pm0.15$              |            $1.04\pm0.02$               |
|  conv    |             $1.48\pm0.02$             |             $2.01\pm0.02$                |  $$\color{lightgreen}0.91\pm0.03$$     |
|  softmax |             $1.83\pm0.04$             |             $1.84\pm0.03$                |  $$\color{lightgreen}0.978\pm0.001$$   |
|  inner   | $${\color{lightgreen}0.737\pm0.014}$$ |             $1.20\pm0.01$                |            $1.29\pm 0.03$              |

Benchdnn did not run for the post operation `eltwise_gelu_erf`, so it was replaced for `eltwise_relu` for the convolution and softmax tests as a approximate reference point. OneDNN used SYCL for kernel invocation which used cudnn for certain operations.


ArrayFire v3.9.0 (oneAPI, 64-bit Linux, build 4061db86e)
[0] Intel(R) OpenCL: Intel(R) Xeon(R) CPU @ 2.30GHz, 32082 MB
|   Test   |           ArrayFire oneAPI            |        OneDNN                          |
|:--------:|:-------------------------------------:|:--------------------------------------:|
|  matmul  |             $9.87\pm0.21$             |  $${\color{lightgreen}4.42\pm0.09}$$   |
|  conv    |             $37.4\pm0.8$              |            $4.61\pm0.16$               |
|  softmax |             $32.4\pm0.4$              |            $3.07\pm0.06$               |
|  inner   | $${\color{lightgreen}14.0\pm0.4}$$    |            $5.59\pm 0.11$              |