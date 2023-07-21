// example.cpp
#include <pybind11/pybind11.h>
#include <iostream>
#include <cmath>

double multiplica(double x, double y) {
    double result = x * y;
    return result;
}


namespace py = pybind11;

PYBIND11_MODULE(GeneticAlgorithm2, m) {
    m.def("multiplica", &multiplica, "A function that sums to elemets");
}

