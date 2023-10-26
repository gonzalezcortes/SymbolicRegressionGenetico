#include <pybind11/embed.h>
#include <iostream>

namespace py = pybind11;

int main() {
    // Initialize the Python interpreter
    py::scoped_interpreter guard{};

    // Import NumPy
    py::module_ np = py::module_::import("numpy");

    // Compute sum of array [1, 2, 3]
    py::object result = np.attr("sum")(py::array_t<int>({ 1, 2, 3 }));

    // Extract the result and print it
    std::cout << "Sum: " << result.cast<int>() << std::endl;

    return 0;
}
