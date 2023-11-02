#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <chrono>
#include <random>
#include "external/omp.h"
#include <iostream>

void myFunction(int id) {
    long long sum = 0;
    for (int i = 0; i <= 1000; ++i) {
        sum += i;
    }
}

std::vector<double> generate_synthetic_data(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0.0, 1.0);

    std::vector<double> synthetic_data(n);
    for (int i = 0; i < n; i++) {
        synthetic_data[i] = dist(gen);
    }

    return synthetic_data;
}

void loop_1(std::vector<double> synthetic_data){
    #pragma omp parallel for
    for (int i = 0; i < synthetic_data.size(); ++i) {
        myFunction(i);
    }
}

void loop_2(std::vector<double> synthetic_data) {

    for (int i = 0; i < synthetic_data.size(); ++i) {
        myFunction(i);
    }
}

PYBIND11_MODULE(parallel_test, m) {
    m.doc() = "Python binding for synthetic data generation and computation";

    m.def("generate_synthetic_data", &generate_synthetic_data, "Generate synthetic data");

    m.def("myFunction", &myFunction, "Simulated computation function");

    m.def("loop_1", &loop_1, "loop_1 Fast");

    m.def("loop_2", &loop_2, "loop_2 Classic");
}
