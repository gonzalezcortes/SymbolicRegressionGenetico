// example.cpp
#include <pybind11/pybind11.h>
#include <iostream>
#include <cmath>

double suma(double x, double y) {
    double result = x + y;
    return result;
}


namespace py = pybind11;

PYBIND11_MODULE(example_1, m) {
    m.def("suma", &suma, "A function that sums to elemets");
}

