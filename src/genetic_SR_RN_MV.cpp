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
    double** y_array;

    int i, j, h, w;
    int rows_x, rows_y, cols_x, cols_y;

    std::vector<std::string> binary_operators;
    std::vector<std::string> unary_operators;
    std::vector<std::string> terminals;
    std::vector<std::string> constants;

public:

    void set_matrix_x_from_numpy(py::array_t<double> input_array) {

        py::buffer_info buf_info = input_array.request();
        h = buf_info.shape[0];
        w = buf_info.shape[1];

        // Dynamic 2D array allocation
        X_array = new double* [h];
        for (int i = 0; i < h; ++i) {
            X_array[i] = new double[w];
        }

        // Pointer to numpy array data
        const double* ptr = static_cast<const double*>(buf_info.ptr);

        // Data into C++ array
        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                X_array[i][j] = ptr[i * w + j];
            }
        }
    }

    void set_matrix_y_from_numpy(py::array_t<double> input_array) {

        py::buffer_info buf_info = input_array.request();
        h = buf_info.shape[0];
        w = buf_info.shape[1];

        y_array = new double* [h];
        for (int i = 0; i < h; ++i) {
            y_array[i] = new double[w];
        }

        const double* ptr = static_cast<const double*>(buf_info.ptr);

        for (int i = 0; i < h; ++i) {
            for (int j = 0; j < w; ++j) {
                y_array[i][j] = ptr[i * w + j];
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

    int random_int(int min, int max) {
        return min + rand() % (max - min + 1);
    }

    void print_operators() {
        std::cout << " Binary operators: " << std::endl;
        for (const auto& op : binary_operators) {
			std::cout << op << " ";}
        std::cout << std::endl;

        std::cout << " Unary operators: " << std::endl;
        for (const auto& op : unary_operators) {
            std::cout << op << " ";}
        std::cout << std::endl;

        std::cout << " Terminals: " << std::endl;
        for (const auto& op : terminals) {
            std::cout << op << " ";}
        std::cout << std::endl;

        std::cout << " Constants: " << std::endl;
        // print them horizontally
        for (const auto& op : constants) {
            std::cout << op << " ";}
        std::cout << std::endl;  
	}


    std::string generate_random_expr(int depth) {
        if (depth > 0 && ((double)rand() / (RAND_MAX)) < 0.5) {
            // 50% chance to choose unary or binary operator
            if ((double)rand() / (RAND_MAX) < 0.5) {
                std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];
                return "(" + generate_random_expr(depth - 1) + " " + op + " " + generate_random_expr(depth - 1) + ")";
            }
            else {
                std::string op = unary_operators[random_int(0, unary_operators.size() - 1)];
                return op + "(" + generate_random_expr(depth - 1) + ")";
            }
        }
        else {
            /// 50% chance to choose a constant or a terminal
            if ((double)rand() / (RAND_MAX) < 0.5) {
				return constants[random_int(0, constants.size() - 1)];
			}
            else {
                return "(" + terminals[random_int(0, terminals.size() - 1)] + ")";
            }
        }
    }

    std::vector<std::string> create_initial_population(int pop_size, int depth) {
        std::vector<std::string> population;
        for (int i = 0; i < pop_size; i++) {
            population.push_back(generate_random_expr(depth));
        }
        return population;
    }


    double evaluateRPN2(const std::vector<std::string>& rpn) {
        std::stack<double> values;
        for (const auto& token : rpn) {
            if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
                double b = values.top(); values.pop();
                double a = values.top(); values.pop();
                double result = 0;
                if (token == "+") result = a + b;
                else if (token == "-") result = a - b;
                else if (token == "*") result = a * b;
                else if (token == "/") {
                    if (b == 0) {
                        // std::cerr << "Division by zero\n";
                        return -1;
                    }
                    result = a / b;
                }
                else if (token == "^") result = std::pow(a, b);
                values.push(result);
            }
            else if (token == "u-") {
                double a = values.top(); values.pop();
                values.push(-a);
            }

            else if (token == "sin") {
                double operand = values.top(); values.pop();
                values.push(sin(operand));
            }

            else if (token == "cos") {
                double operand = values.top(); values.pop();
                values.push(cos(operand));
            }

            else if (token == "exp") {
                double operand = values.top(); values.pop();
                values.push(exp(operand));
            }


            else {
                values.push(std::stod(token));
            }
        }
        return values.top();
    }


    std::vector<double> evaluationArray(std::vector<double> x_values, std::vector<std::string> rpn) {
        int n = x_values.size();
        std::vector<double> evaluation_vector;
        evaluation_vector.reserve(n);

        for (double x : x_values) {
            std::vector<std::string> rpn_copy = rpn;
            for (std::string& token : rpn_copy) {
                if (token == "X") token = std::to_string(x);
            }
            double result = this -> evaluateRPN2(rpn_copy);
            // std::cout << "For X = " << x << ", result = " << result << std::endl;

            if (std::isnan(result)) {
                result = std::numeric_limits<double>::infinity();
            }
            evaluation_vector.push_back(result);
        }
        return evaluation_vector;
    }

    std::vector<double> evaluate_fx_RPN2(std::string expression_str, std::vector<double> x_values) {
        std::vector<std::string> replaced_expressions;
        std::string replaced_str = expression_str;

        std::string infix = addSpaces(replaced_str);
        std::istringstream iss(infix);

        std::vector<std::string> rpn = infixToRPN2(iss);

        //print the RPN
        
        for (const auto& token : rpn) {
			std::cout << token << ' ';
		}
        std::cout << std::endl;
        
        std::vector<double> evaluation_vector = { 1.0, 2.0, 3.0 };
        // std::vector<double> evaluation_vector = this -> evaluationArray(x_values, rpn);

        return evaluation_vector;

    }


    std::vector<std::pair<double, std::string>> sorted_expressions(std::vector<std::string> expressions, std::string metric) {

        std::vector<double> x_values = { 1.0, 2.0, 3.0 };
        std::vector<double> y_values = { 1.0, 2.0, 3.0 };

        std::vector<std::pair<double, std::string>> mse_and_expression; // mse and expressions
        std::vector<std::pair<double, std::string>> elite_expressions;
        double mse_score;

        for (const auto& expr : expressions) {

            // std::vector<double> scores = evaluate_fx(expr, x_values);
            std::vector<double> scores = this -> evaluate_fx_RPN2(expr, x_values);
            mse_score = mse(scores, y_values);

            // std::cout << mse_score << std::endl;

            // reverse the RPN
            mse_and_expression.emplace_back(mse_score, expr);
        }

        std::sort(mse_and_expression.begin(), mse_and_expression.end());

        return mse_and_expression;

    }


    std::vector<std::pair<double, std::string>> genetic_training(int population_size, int depth, int generations, std::string metric, double elite_perc,
        double mutation_prob, double grow_prob) {

        std::vector<std::string> expressions = this->create_initial_population(population_size, depth);
        std::vector<std::pair<double, std::string>> sorted_expressions_vec;
        std::vector<std::string> elite;
        std::vector<std::string> cross_elite;
        std::vector<std::string> new_population;
        std::vector<std::string> mutated_new_population;

        for (int gen = 0; gen < generations; gen++) {
            sorted_expressions_vec = this->sorted_expressions(expressions, metric);

        }
        // std::vector<std::pair<double, std::string>> final_expression_vec = sorted_expressions(expressions, metric, x_values, y_values);
        std::vector<std::pair<double, std::string>> final_expression_vec = sorted_expressions_vec; ///////// for now, change
        return final_expression_vec;
    }
};


PYBIND11_MODULE(geneticSymbolicRegressionRN_MV, m) {

    py::class_<Training>(m, "Training")
        .def(py::init<>())
        .def("set_matrix_x_from_numpy", &Training::set_matrix_x_from_numpy, "Set the matrix x from numpy array")
        .def("set_matrix_y_from_numpy", &Training::set_matrix_y_from_numpy, "Set the matrix y from numpy array")
        .def("set_binary_operators", &Training::set_binary_operators, "Set binary operators")
        .def("set_unary_operators", &Training::set_unary_operators, "Set unary operators")
        .def("set_terminals", &Training::set_terminals, "Set terminals")
        .def("set_constants", &Training::set_constants, "Set constants")
        .def("print_operators", &Training::print_operators, "Print the operators")
        .def("random_int", &Training::random_int, "function that returns a random integer between min and max")
        .def("evaluateRPN2", &Training::evaluateRPN2, "function that evaluates the RPN")
        .def("evaluationArray", &Training::evaluationArray, "function that evaluates the RPN")
        .def("evaluate_fx_RPN2", &Training::evaluate_fx_RPN2, "function that evaluates the RPN")
        .def("sorted_expressions", &Training::sorted_expressions, "function that sorts the expressions")
        .def("generate_random_expr", &Training::generate_random_expr, "function that generates a random expression")
        .def("create_initial_population", &Training::create_initial_population, "function that creates the initial population")
        .def("genetic_training", &Training::genetic_training, "function that runs the training of the symbolic regression using genetic training");
}