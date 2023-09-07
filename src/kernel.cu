#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>

namespace py = pybind11;

void square(std::vector<int> &data) {
    int *dev_data;
    int size = data.size();
  
    cudaMalloc((void **)&dev_data, size * sizeof(int));
    cudaMemcpy(dev_data, data.data(), size * sizeof(int), cudaMemcpyHostToDevice);
  
    square_kernel<<<(size + 255) / 256, 256>>>(dev_data, size);
  
    cudaMemcpy(data.data(), dev_data, size * sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(dev_data);
}

PYBIND11_MODULE(kernel, m) {
    m.def("square", &square, "Square each element of the array");
}
