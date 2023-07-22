#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <cmath>
#include <stdexcept>

double mse(const std::vector<double>& targets, const std::vector<double>& predictions) {
    if (predictions.size() != targets.size()) {
        throw std::invalid_argument("Sizes must be equal.");
    }

    double sum = 0.0;
    for (size_t i = 0; i < predictions.size(); ++i) {
        double error = predictions[i] - targets[i];
        sum += error * error; // square of error
    }

    double mse = sum / predictions.size();
    return mse;
}

namespace py = pybind11;

PYBIND11_MODULE(metrics, m) {
    m.def("mse", &mse, "A function that calculates MSE");
}