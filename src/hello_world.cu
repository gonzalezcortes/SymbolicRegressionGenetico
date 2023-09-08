#include <stdio.h>

// Define a kernel function.
__global__ void helloWorldKernel(int* array, int N) {
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    if (index < N) {
        array[index] = index;
    }
}

extern "C" void run_hello_cuda(int* host_array, int N) {
    int* device_array;

    // Allocate device memory
    cudaMalloc((void**)&device_array, N * sizeof(int));

    // Launch the kernel
    helloWorldKernel << <1, N >> > (device_array, N);

    // Copy the array back to the host
    cudaMemcpy(host_array, device_array, N * sizeof(int), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(device_array);
}
