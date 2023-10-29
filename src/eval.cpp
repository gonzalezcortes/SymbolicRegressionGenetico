
#include <iostream>
#include <stack>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>

std::vector<std::string> binary_operators = {"+", "-", "*", "/" };
std::vector<std::string> unary_operators = {};
std::vector<std::string> terminals = { "X", "Y" };
std::vector<std::string> constants = { "1" };

int lenUnary = unary_operators.size();
double unary_prob;


int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

std::string generate_random_expr(int depth) {

    if (depth > 0 && ((double)rand() / (RAND_MAX)) < 0.9) {
        // 50% chance to choose unary or binary operator

        if (lenUnary > 0) {
            unary_prob = 0.1;
        }
        else {
            unary_prob = 0;
        }

        if ((double)rand() / (RAND_MAX) > unary_prob) {
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
            return "(" + constants[random_int(0, constants.size() - 1)] + ")";
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



int main(){

    std::vector<std::string> ipp;
    std::string single_expression;

    int depth = 3;
    int population_size = 10;
    
    single_expression = generate_random_expr(depth);

    ipp = create_initial_population(population_size, depth);
    

    for (auto i : ipp) {
        std::cout << i << std::endl;
    }

    return 0;


}