#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream> 
#include "./external/exprtk.hpp"
#include "metrics.cpp" // metric from mse

std::vector<std::string> operators = { "+", "-", "*", "/" };
std::vector<std::string> terminals = { "x", "1", "2", "3"};


int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Function that generates random expressions
std::string generate_random_expr(int depth) {
    if (depth > 0 && ((double)rand() / (RAND_MAX)) < 0.5) {
        std::string op = operators[random_int(0, operators.size() - 1)];
        return "(" + generate_random_expr(depth - 1) + " " + op + " " + generate_random_expr(depth - 1) + ")";
    }
    else {
        return terminals[random_int(0, terminals.size() - 1)];
    }
}

// Function to create an initial population
std::vector<std::string> create_initial_population(int pop_size, int depth) {
    std::vector<std::string> population;
    for (int i = 0; i < pop_size; i++) {
        population.push_back(generate_random_expr(depth));
    }
    return population;
}

// Function to evaluate an expression
std::vector<double> evaluate_fx(std::string expression_str, std::vector<double> x_values) {
    double x;

    exprtk::symbol_table<double> symbol_table;
    symbol_table.add_variable("x", x);
    exprtk::expression<double> expression;
    exprtk::parser<double> parser;

    int n = x_values.size();
    std::vector<double> evaluation_vector;
    evaluation_vector.reserve(n);

    expression.register_symbol_table(symbol_table);
    if (parser.compile(expression_str, expression))
    {
        for (double x_value : x_values) {
            x = x_value;
            double result = expression.value();
            // std::cout << "The result of the expression for x = " << x << " is: " << result << std::endl;
            evaluation_vector.push_back(result);
        }
        return evaluation_vector;
    }
    else {
        std::cout << "There is an error in the evaluation of the function " << expression_str << std::endl;
    }
    
}


namespace py = pybind11;

PYBIND11_MODULE(geneticSymbolicRegression, m) {
    m.def("generate_random_expr", &generate_random_expr, "A function that generates expressions");
    m.def("create_initial_population", &create_initial_population, "A function to create an initial population");

    m.def("evaluate_fx", &evaluate_fx, "A function to evaluate an expression");
}