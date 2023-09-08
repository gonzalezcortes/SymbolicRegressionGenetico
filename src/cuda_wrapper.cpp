#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>

extern "C" void run_hello_world_kernel(int* host_array, int N);

namespace py = pybind11;

void run_hello_world() {
    std::vector<int> host_array(16);
    run_hello_world_kernel(host_array.data(), host_array.size());

    py::print("Hello from CUDA!");
    py::print("Array:", host_array);
}

PYBIND11_MODULE(cuda_kernel, m) {
    m.def("run_hello_world", &run_hello_world, "Run the hello world CUDA kernel");
}
