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
    double** y_array; // empty for now

    //double** y_vector;
    std::vector<double> y_vector;

    int i, j, h, w;
    int rows_x, rows_y, cols_x, cols_y;

    std::vector<std::string> binary_operators;
    std::vector<std::string> unary_operators;
    std::vector<std::string> terminals;
    std::vector<std::string> constants;

public:

    void set_matrix_x_from_numpy(py::array_t<double> input_array) {

        py::buffer_info buf_info = input_array.request();
        rows_x = buf_info.shape[0];
        cols_x = buf_info.shape[1];

        // Dynamic 2D array allocation
        X_array = new double* [rows_x];
        for (int i = 0; i < rows_x; ++i) {
            X_array[i] = new double[cols_x];
        }

        // Pointer to numpy array data
        const double* ptr = static_cast<const double*>(buf_info.ptr);

        // Data into C++ array
        for (int i = 0; i < rows_x; ++i) {
            for (int j = 0; j < cols_x; ++j) {
                X_array[i][j] = ptr[i * cols_x + j];
            }
        }

        //print X_array
        /*
        std::cout << "X_array" << std::endl;
        for (int i = 0; i < rows_x; i++) {
            for (int j = 0; j < cols_x; j++) {
                std::cout << X_array[i][j] << ' ';
            }
            std::cout << std::endl;
        }*/
    }
    /*
    void set_matrix_y_from_numpy(py::array_t<double> input_array) {

        py::buffer_info buf_info = input_array.request();
        rows_y = buf_info.shape[0];
        cols_y = buf_info.shape[1];

        y_array = new double* [rows_y];
        for (int i = 0; i < rows_y; ++i) {
            y_array[i] = new double[cols_y];
        }

        const double* ptr = static_cast<const double*>(buf_info.ptr);

        for (int i = 0; i < rows_y; ++i) {
            for (int j = 0; j < cols_y; ++j) {
                y_array[i][j] = ptr[i * cols_y + j];
            }
        }
    }
    */



    void set_matrix_y_from_numpy(py::array_t<double> input_array) {
        py::buffer_info buf_info = input_array.request();
        rows_y = buf_info.shape[0];
        cols_y = buf_info.shape[1];

        // Resize the 1D vector to fit all the 2D data
        y_vector.resize(rows_y * cols_y);

        const double* ptr = static_cast<const double*>(buf_info.ptr);

        for (int i = 0; i < rows_y; ++i) {
            for (int j = 0; j < cols_y; ++j) {
                y_vector[i * cols_y + j] = ptr[i * cols_y + j];
            }
        }

        // Print y_vector
        /*
        std::cout << "y_vector" << std::endl;
        int idx = 0;
        for (int i = 0; i < rows_y; i++) {
            for (int j = 0; j < cols_y; j++) {
                std::cout << y_vector[idx++] << ' ';
            }
            std::cout << std::endl;
        }
        */

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


    std::vector<double> evaluationArray(std::vector<std::string> rpn) {

        // print the RPN
        std::cout << "rpn";
        for (const auto& tokenr : rpn) {
			std::cout << tokenr << ' ';
		}
        std::cout << std::endl;
        
        std::vector<double> evaluation_vector; // vector with the evaluation of the RPN
        evaluation_vector.reserve(rows_x); // reserve space for the vector

        std::vector<std::string> rpn_copy = rpn; // copy the RPN
        int n_terminals = terminals.size(); // number of terminals or variabales

        // loop the row
        for (int i = 0; i < rows_x; i++) {
            for (std::string& token : rpn_copy) { // loop the RPN
                int position = std::find(terminals.begin(), terminals.end(), token) - terminals.begin(); // find the position of the token in the terminals vector

                std::cout << " i " << i << "position " << position << ' ';

                if (position < n_terminals) {
                    int position_index = position-1;
                    double value = X_array[i][position_index]; // get the value of the dataset
                    std::cout << " token " << token << " cambiado po value " << value << ' ';
                    token = std::to_string(value); // replace the token with the value of the dataset
                }
                
                else {token = token;} // if the token is not a terminal, keep it

                std::cout << std::endl;
            }

            //print rpn
            /*
            for (const auto& token2 : rpn) {
				std::cout << token2 << ' ';
			}
            std::cout << std::endl;*/

            double result = this->evaluateRPN2(rpn_copy); // evaluate the RPN
            if (std::isnan(result)) { // if the result is nan, set it to infinity
                result = std::numeric_limits<double>::infinity();
            }
            evaluation_vector.push_back(result); // push the result in the evaluation vector
        }

        /*
        // print rpn
        for (const auto& token2 : rpn) {
            std::cout << token2 << ' ';
        }
        std::cout << std::endl;

        // print rpn copy
        for (const auto& token3 : rpn_copy) {
			std::cout << token3 << ' ';
		}
        std::cout << std::endl;

        // print evaluation vector
        for (const auto& token4 : evaluation_vector) {
            std::cout << token4 << ' ';
        }
        std::cout << std::endl;
        std::cout << "#########" << std::endl;

        */

        return evaluation_vector;
    }

    std::vector<double> evaluationArray2(std::vector<std::string> rpn) {
        
        /*
        std::cout << "rpn: ";
        for (const auto& token : rpn) {
            std::cout << token << ' ';
        }
        std::cout << std::endl;
        */

        std::vector<double> evaluation_vector;
        evaluation_vector.reserve(rows_x);

        int n_terminals = terminals.size();

        for (int i = 0; i < rows_x; i++) {
            std::vector<std::string> rpn_copy = rpn;  // Create a fresh copy each time
            for (std::string& token : rpn_copy) {
                auto it = std::find(terminals.begin(), terminals.end(), token);
                if (it != terminals.end()) {
                    int position = it - terminals.begin();
                    double value = X_array[i][position];

                    // std::cout << token << " changed by " << value << '\n';

                    token = std::to_string(value);

                }
            }

            /*
            std::cout << "new rpn (rpn_copy): ";
            for (const auto& token : rpn_copy) {
                std::cout << token << ' ';
            }
            */

            double result = this->evaluateRPN2(rpn_copy); // evaluate the RPN
            if (std::isnan(result)) { // if the result is nan, set it to infinity
                result = std::numeric_limits<double>::infinity();
            }
            evaluation_vector.push_back(result); // push the result in the evaluation vector

            // std::cout << result << " ";
            // std::cout << std::endl;
        }
        return evaluation_vector;
    }


    std::vector<double> evaluate_fx_RPN2(std::string expression_str) {

        // print the expression
        // std::cout << "expression " <<expression_str << std::endl;

        std::vector<std::string> replaced_expressions; // vector with the replaced expressions
        std::string replaced_str = expression_str; // string with the replaced expression

        std::string infix = addSpaces(replaced_str); // add spaces to the expression
        std::istringstream iss(infix); // create a string stream with the expression

        std::vector<std::string> rpn = infixToRPN2(iss); // convert the expression to RPN

        //print the RPN
        /*
        for (const auto& token : rpn) {
			std::cout << token << ' ';
		}
        std::cout << std::endl;
        */
        

        std::vector<double> evaluation_vector = this -> evaluationArray2(rpn);

        return evaluation_vector;

    }

    std::vector<std::string> get_elite(std::vector<std::pair<double, std::string>> sorted_expressions, double elite_perc) {

        std::vector<std::string> elite_expressions;
        size_t elite_count = static_cast<size_t>(std::ceil(elite_perc * sorted_expressions.size()));

        elite_expressions.reserve(elite_count);
        for (size_t i = 0; i < elite_count; ++i) {
            elite_expressions.push_back(sorted_expressions[i].second); // Get the string part of the pair
        }

        return elite_expressions;
    }




    std::string merge_expressions(const std::string& expr1, const std::string& expr2) {
        std::string new_expr1 = expr1;
        std::string new_expr2 = expr2;

        /*
        if (expr1.front() != '(' && expr1.back() != ')') {
            new_expr1 = "(" + expr1 + ")";
        }
        if (expr2.front() != '(' && expr2.back() != ')') {
            new_expr2 = "(" + expr2 + ")";
        }
        */

        size_t midpoint1 = new_expr1.find_last_of(')');
        size_t midpoint2 = new_expr2.find_first_of('(');

        std::string merged_expr = new_expr1.substr(0, midpoint1 + 1) + new_expr2.substr(midpoint2);

        size_t merge_point = new_expr1.substr(0, midpoint1 + 1).size();

        int n_binary_operators = binary_operators.size();
        int indexReplace = rand() % n_binary_operators;
        std::string replaceStr = binary_operators[indexReplace];

        if (merged_expr[merge_point - 1] == ')' && merged_expr[merge_point] == '(') {
            merged_expr.insert(merge_point, replaceStr);
        }

        return merged_expr;
    }

    std::vector<std::string> cross_expressions(const std::vector<std::string>& elite_expressions) {
        std::vector<std::string> crossed_expressions;
        int loop_limit = elite_expressions.size() - 1;

        for (int i = 0; i < loop_limit; i++) {
            std::string crossed_expr = this -> merge_expressions(elite_expressions[i], elite_expressions[i + 1]);
            crossed_expressions.push_back(crossed_expr);
        }

        return crossed_expressions;
    }

    std::vector<std::string> get_new_population(std::vector<std::string>& crossed_expressions, std::vector<std::string>& elite_expressions, int population_size, int depth) {

        std::vector<std::string> new_expressions;
        new_expressions.reserve(population_size);


        // elite expressions
        for (const auto& elite_expr : elite_expressions) {
            new_expressions.push_back(elite_expr);
        }

        // crossed over expressions
        for (const auto& crossed_expr : crossed_expressions) {
            new_expressions.push_back(crossed_expr);
        }

        //  new random expressions
        while (new_expressions.size() < population_size) {
            new_expressions.push_back(generate_random_expr(depth));
        }

        return new_expressions;
    }

    std::vector<std::pair<double, std::string>> sorted_expressions(std::vector<std::string> expressions, std::string metric) {


        //print expressions
        /*
        for (const auto& token : expressions) {
			std::cout << token << '\n';
		}
        std::cout << std::endl;
        */

        std::vector<std::pair<double, std::string>> mse_and_expression; // mse and expressions
        std::vector<std::pair<double, std::string>> elite_expressions;
        double mse_score;

        for (const auto& expr : expressions) {

            // std::vector<double> scores = evaluate_fx(expr, x_values);
            std::vector<double> scores = this -> evaluate_fx_RPN2(expr);

            // print expression and scores
            /*
            std::cout << "expression: " << expr << std::endl;
            for (const auto& token : scores) {
				std::cout << token << ' ';
			}
            */

            //mse_score = 1.0; // for now (delete)
            mse_score = mse(y_vector, scores); ////

            //std::cout << "mse: " << mse_score << std::endl;


            // reverse the RPN
            mse_and_expression.emplace_back(mse_score, expr);
        }

        std::sort(mse_and_expression.begin(), mse_and_expression.end());

        return mse_and_expression;

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

    // Function to modify expression (binary and unary operators and terminals)
    std::string modify_expression(std::string expr) {
        int n_binary_operators = binary_operators.size();
        int n_unary_operators = unary_operators.size();
        int n_terminals = terminals.size();
        int n_constants = constants.size();

        int indexToSearch;
        std::string toSearch;
        int indexReplace;
        std::string replaceStr;
        std::size_t pos;

        //////////////////////
        // binary_operators //
        //////////////////////
        
        indexToSearch = rand() % n_binary_operators;
        toSearch = binary_operators[indexToSearch];

        indexReplace = rand() % n_binary_operators;
        replaceStr = binary_operators[indexReplace];

        pos = expr.find(toSearch);
        if (pos != std::string::npos) {
            expr.replace(pos, 1, replaceStr);
        }
        //////////////////////
        // unary_operators //
        //////////////////////

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

        /*
        int n_terminals = terminals.size();
        indexToSearch = rand() % n_terminals;
        toSearch = terminals[indexToSearch];

        indexReplace = rand() % n_terminals;
        replaceStr = terminals[indexReplace];

        pos = expr.find(toSearch);
        if (pos != std::string::npos) {
            expr.replace(pos, 1, replaceStr);
        }
        //////////////////////
        // constants //
        //////////////////////
        int n_constants = constants.size();
        indexToSearch = rand() % n_constants;
        toSearch = constants[indexToSearch];

        indexReplace = rand() % n_constants;
        replaceStr = constants[indexReplace];

        pos = expr.find(toSearch);
        if (pos != std::string::npos) {
            expr.replace(pos, 1, replaceStr);
        }
        */

        indexToSearch = std::rand() % n_terminals;
        
        toSearch = terminals[indexToSearch];
        // std::cout << "Searching for " << toSearch << std::endl;

        if (expr.find(toSearch) != std::string::npos) {
            if ((double)rand() / (RAND_MAX) < 0.25) {
                indexReplace = std::rand() % n_constants;
                replaceStr = constants[indexReplace];
                // std::cout << "Expression " << expr << std::endl;
                // std::cout << "Replacing " << toSearch << " with " << replaceStr << std::endl;
            }
            else {
                indexReplace = std::rand() % n_terminals;
                replaceStr = terminals[indexReplace];
                // std::cout << "Expression " << expr << std::endl;
                // std::cout << "Replacing " << toSearch << " with " << replaceStr << std::endl;
            }
        }

        pos = expr.find(toSearch);
        if (pos != std::string::npos) {
            expr.replace(pos, 1, replaceStr);
        }

        return expr;
    }

    std::vector<std::string> mutation(const std::vector<std::string>& expressions, double mutation_prob, double elite_perc, double grow_prob) {

        int population_size = expressions.size();
        std::vector<std::string> new_expressions;
        new_expressions.reserve(population_size);
        std::string mutated_expr;

        //print new_expressions



        // Do not touch elite expressions
        size_t elite_count = static_cast<size_t>(std::ceil(elite_perc * expressions.size()));
        for (size_t i = 0; i < elite_count; ++i) {
            new_expressions.push_back(expressions[i]);
        }

        // Apply mutation with a certain probability
        for (size_t i = elite_count; i < population_size; ++i) {
            double rand_prob = ((double)rand() / (RAND_MAX));

            // mutation if prob is lower than mutation_prob
            if (rand_prob < mutation_prob) {
                mutated_expr = modify_expression(expressions[i]);
                new_expressions.push_back(mutated_expr);
            }
            else if (rand_prob < grow_prob) {
                if ((double)rand() / (RAND_MAX) < 0.5) {
                    mutated_expr = add_term_left(expressions[i]);
                    new_expressions.push_back(mutated_expr);
                }
                else {
                    mutated_expr = add_term_right(expressions[i]);
                    new_expressions.push_back(mutated_expr);
                }
            }
            else {
                // mutated_expr = expressions[i];
                new_expressions.push_back(expressions[i]);
            }

            // std::cout << mutated_expr << std::endl;
        }

        return new_expressions;
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

            
            // std::cout << "Generation: " << gen << std::endl;
            // for (const auto& token : expressions) { std::cout << token << '\n'; }
            // std::cout << std::endl;
            // std::cout << "##########" << std::endl;
            
            sorted_expressions_vec = this->sorted_expressions(expressions, metric);
            elite = this -> get_elite(sorted_expressions_vec, elite_perc); 
            //print elite
            
            /*
            for(const auto& token : elite) {    
                std::cout << token << ' ';
                std::vector<double> scores2 = this->evaluate_fx_RPN2(token); // borrar
               
                double mse_score2 = mse(scores2, y_vector); // borrar
                std::cout << "mse: " << mse_score2 << std::endl; // borrar
            }
            */

            cross_elite = this -> cross_expressions(elite);
            
            new_population = this -> get_new_population(elite, cross_elite, population_size, depth);
            mutated_new_population = this -> mutation(new_population, mutation_prob, elite_perc, grow_prob);

            //expressions = simplify_all_expr(mutated_new_population); (not implemented yet)
            expressions = mutated_new_population;

            // print the expressions
            /*
            std::cout << "Get mutated population" << std::endl;
            for (const auto& token : expressions) {std::cout << token << '\n';}
            std::cout << std::endl;
            std::cout << "#####FIN#####" << std::endl;*/

        }
        std::vector<std::pair<double, std::string>> final_expression_vec = sorted_expressions(expressions, metric);

        // std::vector<std::pair<double, std::string>> final_expression_vec = sorted_expressions_vec; ///////// for now, change
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
        .def("evaluationArray2", &Training::evaluationArray2, "function that evaluates the RPN")
        .def("evaluate_fx_RPN2", &Training::evaluate_fx_RPN2, "function that evaluates the RPN")
        .def("sorted_expressions", &Training::sorted_expressions, "function that sorts the expressions")
        .def("get_elite", &Training::get_elite, "function that gets the elite")
        .def("cross_expressions", &Training::cross_expressions, "function that crosses the expressions")
        .def("merge_expressions", &Training::merge_expressions, "function that merges the expressions")
        .def("mutation", &Training::mutation, "function that mutates the expressions")
        .def("get_new_population", &Training::get_new_population, "function that gets the new population")
        .def("generate_random_expr", &Training::generate_random_expr, "function that generates a random expression")
        .def("create_initial_population", &Training::create_initial_population, "function that creates the initial population")
        .def("genetic_training", &Training::genetic_training, "function that runs the training of the symbolic regression using genetic training");
}