#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <limits>
#include <sys/types.h>



#include "./external/exprtk.hpp"
#include "metrics.cpp" // metric from mse

#include "reverseNotation.cpp"

namespace py = pybind11;

class Training {
private:
    double** X_array;
    float matrix_y;

    int h, w;
    int i, j;

    int rows_x;
    int cols_x;
    int rows_y;
    int cols_y;

    std::vector<std::string> binary_operators;
    std::vector<std::string> unary_operators;
    std::vector<std::string> terminals;
    std::vector<std::string> constants;

public:

    void set_matrix_x_from_numpy(py::array_t<double> input_array) {
        // Get buffer info from numpy array
        py::buffer_info buf_info = input_array.request();
        h = buf_info.shape[0];
        w = buf_info.shape[1];

        // Allocate dynamic 2D array
        X_array = new double* [h];
        for (int i = 0; i < h; ++i) {
            X_array[i] = new double[w];
        }

        // Get pointer to numpy array data
        const double* ptr = static_cast<const double*>(buf_info.ptr);

        // Copy data into C++ array
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                X_array[i][j] = ptr[i * w + j];
            }
        }
    }


    void set_binary_operators(std::vector<std::string> new_binary_operators) {
        binary_operators = new_binary_operators;
    }
    
    void set_unary_operators(std::vector<std::string> new_unary_operators) {
		unary_operators = new_unary_operators;
	}

    void set_terminals(std::vector<std::string> new_terminals) {
        terminals = new_terminals;
    }

    void set_constants(std::vector<std::string> new_constants) {
		constants = new_constants;
	}

    std::vector<std::pair<double, std::string>> genetic_training(int population_size, int depth, int generations, std::string metric, double elite_perc,
        double mutation_prob, double grow_prob) {

        std::vector<std::string> expressions = create_initial_population(population_size, depth);

        std::vector<std::pair<double, std::string>> final_expression_vec;


        return final_expression_vec;
    }
};





PYBIND11_MODULE(geneticSymbolicRegressionRN_MV, m) {

    py::class_<Training>(m, "Training")
        .def(py::init<>())
        .def("set_matrix_x_from_numpy", &Training::set_matrix_x_from_numpy, "Set the matrix x from numpy array")
        .def("set_binary_operators", &Training::set_binary_operators, "Set binary operators")
        .def("set_unary_operators", &Training::set_unary_operators, "Set unary operators")
        .def("set_terminals", &Training::set_terminals, "Set terminals")
        .def("set_constants", &Training::set_constants, "Set constants")
        .def("genetic_training", &Training::genetic_training, "function that runs the training of the symbolic regression using genetic training");

}