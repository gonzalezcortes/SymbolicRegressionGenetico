#include <stdio.h>

// Define a kernel function.
__global__ void helloWorldKernel(int* array, int N) {
    int index = threadIdx.x + blockIdx.x * blockDim.x;
    if (index < N) {
        array[index] = index;
    }
}

int main() {
    int N = 16; // The size of our array
    int* host_array, * device_array;

    // Allocate host memory
    host_array = (int*)malloc(N * sizeof(int));

    // Allocate device memory
    cudaMalloc((void**)&device_array, N * sizeof(int));

    // Launch the kernel
    helloWorldKernel << <1, N >> > (device_array, N);

    // Copy the array back to the host
    cudaMemcpy(host_array, device_array, N * sizeof(int), cudaMemcpyDeviceToHost);

    // Print out the resulting array
    printf("Hello from CUDA!\nArray: ");
    for (int i = 0; i < N; i++) {
        printf("%d ", host_array[i]);
    }
    printf("\n");

    // Free device memory
    cudaFree(device_array);

    // Free host memory
    free(host_array);

    return 0;
}
