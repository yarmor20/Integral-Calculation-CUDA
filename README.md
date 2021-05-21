# Integral-Calculation-CUDA

Calculating the integral of [Langermann Function](https://www.sfu.ca/~ssurjano/langer.html) using the advantages of CUDA programing.

Team: Yaroslav Morozevych, Denys Herasymuk, Dmytro Lopushanskyy.
Option: 4

## Runnig the code

The code was run with `Visual Studio` on the graphic card:
```bash
GeForce GTX 960M
Compute Capability: 5.0
SM: 5.0
```

## Approach

For the thread blocks we decided to choose the y-axis and for the threads - the x-axis, such that the kernel launch looks like:
```cuda
#define BLOCK_NUM 256
#define THREAD_NUM 256

integrateKernel<<<BLOCK_NUM, THREAD_NUM>>>(*args)
```
This approach is not as decent as we would like it to be, that is why the whole code is likely to be rewritten with Thrust library in order to improve its readability and resource management.
