#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <algorithm>

#include "./external/exprtk.hpp"
#include "metrics.cpp" // metric from mse

std::vector<std::string> binary_operators = { "+", "-", "*", "/" };
std::vector<std::string> unary_operators = { "sin", "cos" , "exp"};
std::vector<std::string> terminals = { "x", "1", "2", "3" };


int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

// Function that generates random expressions
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
        return std::vector<double>();
    }
    
}

// Function to modify expression (binary and unary operators and terminals)
std::string modify_expression(std::string expr) {
    //////////////////////
    // binary_operators //
    //////////////////////
    int n_binary_operators = 4;
    int indexToSearch = rand() % n_binary_operators;
    std::string toSearch = binary_operators[indexToSearch];

    int indexReplace = rand() % n_binary_operators;
    std::string replaceStr = binary_operators[indexReplace];

    size_t pos = expr.find(toSearch);
    if (pos != std::string::npos) {
        expr.replace(pos, 1, replaceStr);
    }
    //////////////////////
    // unary_operators //
    //////////////////////
    int n_unary_operators = 3;
    indexToSearch = rand() % n_unary_operators;
    toSearch = unary_operators[indexToSearch];

    indexReplace = rand() % n_unary_operators;
    replaceStr = unary_operators[indexReplace];


    pos = expr.find(toSearch);
    if (pos != std::string::npos) {
        // Replace it with "cos" function
        expr.replace(pos, 3, replaceStr);
    }
    //////////////////////
    // terminals //
    //////////////////////
    int n_terminals = 4;
    indexToSearch = rand() % n_terminals;
    toSearch = terminals[indexToSearch];

    indexReplace = rand() % n_terminals;
    replaceStr = terminals[indexReplace];

    pos = expr.find(toSearch);
    if (pos != std::string::npos) {
        expr.replace(pos, 1, replaceStr);
    }
    return expr;
}

// Add term left
std::string add_term_left(std::string expr) {
    // Generate a new term with depth 1 (either a unary operation or a simple binary operation)
    std::string new_term = generate_random_expr(1);

    // Add the new term to the existing expression using a random operator
    std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];

    return "(" + new_term + " " + op + " " + expr + ")";
}

// Add term right
std::string add_term_right(std::string expr) {
    // Generate a new term with depth 1 (either a unary operation or a simple binary operation)
    std::string new_term = generate_random_expr(1);

    // Add the new term to the existing expression using a random operator
    std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];

    return "(" + expr + " " + op + " " + new_term + ")";
}

std::vector<std::pair<double, std::string>> get_best_expressions(std::vector<std::string> expressions, double elite_perc,
    std::string metric, std::vector<double> x_values, std::vector<double> y_values) {

    std::vector<std::pair<double, std::string>> mse_and_expression; // mse and expressions
    std::vector<std::pair<double, std::string>> elite_expressions;
    double mse_score;

    for (const auto& expr : expressions) {
        std::vector<double> scores = evaluate_fx(expr, x_values);
        
        mse_score = mse(scores, y_values);

        // std::cout << mse_score << std::endl;
        mse_and_expression.emplace_back(mse_score, expr);
    }

    std::sort(mse_and_expression.begin(), mse_and_expression.end());

    size_t elite_count = static_cast<size_t>(std::ceil(elite_perc * mse_and_expression.size()));
    
    elite_expressions.reserve(elite_count);
    for (size_t i = 0; i < elite_count; ++i) {
        elite_expressions.push_back(mse_and_expression[i]);
    }

    return elite_expressions;
}

std::string merge_expressions(const std::string& expr1, const std::string& expr2) {
    size_t midpoint1 = expr1.find_last_of(')');
    size_t midpoint2 = expr2.find_first_of('(');
    return expr1.substr(0, midpoint1 + 1) + expr2.substr(midpoint2);
}


namespace py = pybind11;

PYBIND11_MODULE(geneticSymbolicRegression, m) {
    m.def("generate_random_expr", &generate_random_expr, "A function that generates expressions");
    m.def("create_initial_population", &create_initial_population, "A function to create an initial population");

    m.def("evaluate_fx", &evaluate_fx, "A function to evaluate an expression");
    m.def("modify_expression", &modify_expression, "a function to modify expressions");

    m.def("add_term_left", &add_term_left, "a function to add term in the left side of the expression");
    m.def("modify_expression", &add_term_right, "a function to add term in the right side of the expression");
    m.def("get_best_expressions", &get_best_expressions, "function to get the best expression in terms of a given metric");
    m.def("merge_expressions", &merge_expressions, "function that takes some parts of a function and merge them");
}