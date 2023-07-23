//#include <pybind11/pybind11.h>
//#include <pybind11/stl.h>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>

#include "./external/exprtk.hpp"

std::vector<std::string> binary_operators = { "+", "-", "*", "/" };
std::vector<std::string> unary_operators = { "sin", "cos" , "exp"};
std::vector<std::string> terminals = { "y", "1", "2", "3" };



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
    indexToSearch = rand()%n_unary_operators;
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

std::string add_term_left(std::string expr) {
    // Generate a new term with depth 1 (either a unary operation or a simple binary operation)
    std::string new_term = generate_random_expr(1);

    // Add the new term to the existing expression using a random operator
    std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];

    return "(" + new_term + " " + op + " " +  expr + ")";
}

std::string add_term_right(std::string expr) {
    // Generate a new term with depth 1 (either a unary operation or a simple binary operation)
    std::string new_term = generate_random_expr(1);

    // Add the new term to the existing expression using a random operator
    std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];

    return "(" + expr + " " + op + " " + new_term + ")";
}

int main(){

    std::string expressions;
    std::string exp_1 = "(y + 3)";
    std::string exp_2 = "(y + 3) + sin(3) + cos(3)";
    std::string exp_3;
    std::string exp_4;
    std::string exp_5;

    expressions = generate_random_expr(3);
    std::cout << expressions << std::endl;
    srand(static_cast<unsigned int>(time(0))); // Seed random number generator

    std::cout << "Original: " << exp_1 << std::endl;


    std::cout << "Modify: " << modify_expression(exp_1) << std::endl;


    for (int i; i<1000; i++){
        std::cout << "--start--" << std::endl;
        std::cout << exp_2 << std::endl;
        exp_3 = modify_expression(exp_2);
        std::cout << exp_3 << std::endl;
        exp_4 = add_term_left(exp_3);
        std::cout << exp_4 << std::endl;
        exp_5 = add_term_right(exp_4);
        std::cout << exp_5 << std::endl;
        std::cout << modify_expression(exp_5) << std::endl;
        std::cout << "--end--" << std::endl;
    }

    return 0;



}
