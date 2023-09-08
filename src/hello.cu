#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <stdio.h>

__global__ void sayHello() {
    printf("Hello world from the GPU!\n");
}

void launchSayHello() {
    sayHello << <1, 1 >> > ();  // Launch kernel with 1 block and 1 thread
    cudaDeviceSynchronize();  // Make sure the kernel has finished
}

PYBIND11_MODULE(kernel, m) {
    m.def("sayHello", &launchSayHello, "A function to say hello from the GPU");
}
