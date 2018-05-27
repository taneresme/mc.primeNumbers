### Multi-Core / searching for prime numbers with OpenMP

This repo cantains openMP and Thrust implementations for finding prime numbers. Dev-C++ can be used for compiling project. 

When compiling openMP versions, do not forget providing `-fopenmp` flag to compiler.

Sample thrust compiling command will look like `C:\THRUST>nvcc -O2 -o prime_numbers prime_numbers.cu -DTHRUST_DEVICE_SYSTEM=THRUST_DEVICE_SYSTEM_CUDA`
