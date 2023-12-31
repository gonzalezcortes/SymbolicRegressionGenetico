#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

#include <pybind11/embed.h>

#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <algorithm>
#include <limits>
#include <sys/types.h>

#include <unordered_map>

#include <cmath> // for std::isnan

#include "./external/exprtk.hpp"
#include "metrics.cpp" // metric from mse

#include "reverseNotation.cpp"

#include <chrono>

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

    int lenUnary; 
    double unary_prob;

    py::object sympify = py::module::import("sympy").attr("sympify");
    py::object simpify = py::module();

    // py::object simpi py::module::import("sympy").attr("expand");
    // py::object simpi = py::module();

    //py::object simpi = py::module::import("sympy").attr("expand")

    

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

    int random_int2(int min, int max) {
        return min + (rand() % static_cast<int>(max - min + 1));
    }

    double get_random() {
        return static_cast<double>(rand()) / RAND_MAX;
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

    std::string balance_parent(std::string expr) {

        int count_open = std::count(expr.begin(), expr.end(), '(');
        int count_close = std::count(expr.begin(), expr.end(), ')');

        if (count_open > count_close) {
            for (int i = 0; i < (count_open - count_close); ++i) {
                expr += ')';
            }
        }
        else if (count_close > count_open) {
            for (int i = 0; i < (count_close - count_open); ++i) {
                expr = '(' + expr;
            }
        }
        return expr;
    }

    std::string generate_random_expr(int depth) {
        int lenUnary = unary_operators.size();
        double unary_prob = lenUnary > 0 ? 0.1 : 0.0;

        if (depth > 0 && static_cast<double>(rand()) / RAND_MAX < 0.9) {
            if (static_cast<double>(rand()) / RAND_MAX >= unary_prob) {
                std::string op = binary_operators[random_int2(0, binary_operators.size() - 1)];
                return "(" + generate_random_expr(depth - 1) + " " + op + " " + generate_random_expr(depth - 1) + ")";
            }
            else {
                std::string op = unary_operators[random_int2(0, unary_operators.size() - 1)];
                return op + "(" + generate_random_expr(depth - 1) + ")";
            }
        }
        else {
            if (static_cast<double>(rand()) / RAND_MAX < 0.5) {
                return "(" + constants[random_int2(0, constants.size() - 1)] + ")";
            }
            else {
                return "(" + terminals[random_int2(0, terminals.size() - 1)] + ")";
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

    std::vector<double> evaluationArray2(std::vector<std::string> rpn) {

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
            }*/

            double result = evaluateRPN2(rpn_copy); // evaluate the RPN
            if (std::isnan(result)) { // if the result is nan, set it to infinity
                result = std::numeric_limits<double>::infinity();
            }
            evaluation_vector.push_back(result); // push the result in the evaluation vector

            // std::cout << result << " ";
            // std::cout << std::endl;
        }
        return evaluation_vector;
    }


    std::vector<double> evaluationArray2a(std::vector<std::string> rpn) {

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
            }*/
            
            
            double result = evaluateRPN2(rpn_copy); // evaluate the RPN
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

        // std::cout << "Add spaces" << std::endl;
        std::string infix = addSpaces(replaced_str); // add spaces to the expression
        std::istringstream iss(infix); // create a string stream with the expression

        // std::cout << "infix RPN" << expression_str << std::endl;
        std::vector<std::string> rpn = infixToRPN3(iss); // convert the expression to RPN

        //print the RPN

        /*
        for (const auto& token : rpn) {
			std::cout << token << ' ';
		}
        std::cout << std::endl;*/

        
        //std::cout << "Evaluation Array" << " ";
        std::vector<double> evaluation_vector = this -> evaluationArray2(rpn);
        //std::cout << "completed " << std::endl;

        return evaluation_vector;

    }

    std::vector<std::string> get_elite(std::vector<std::pair<double, std::string>> sorted_expressions, double elite_perc) {

        std::vector<std::string> elite_expressions; 
        size_t elite_count = static_cast<size_t>(std::ceil(elite_perc * sorted_expressions.size())); // number of elite expressions

        elite_expressions.reserve(elite_count);
        for (size_t i = 0; i < elite_count; ++i) { // for each elite expression
            elite_expressions.push_back(sorted_expressions[i].second); // push the expression in the elite expressions vector
        }
        /*
        std::cout << "Elite: " << std::endl;
        for (auto i : elite_expressions) {
            std::cout << i << std::endl;
        }*/

        return elite_expressions;
    }

    std::string merge_expressions2(const std::string& expr1, const std::string& expr2) {

        // from binary_operators pick one random operator
        int n_binary_operators = binary_operators.size();
        int indexReplace0 = rand() % n_binary_operators;

        std::string replaceStr0 = binary_operators[indexReplace0];

        std::vector<std::string> binary_operators_1;
        std::vector<std::string> binary_operators_2;

        std::string child;
      

        for (const std::string& bop : binary_operators) {
            if (expr1.find(bop) != std::string::npos) {
                binary_operators_1.push_back(bop);
            }
            if (expr2.find(bop) != std::string::npos) {
                binary_operators_2.push_back(bop);
            }
        }
        
        // if terminals_1 is not empty
        if (!binary_operators_1.empty() && !binary_operators_2.empty()) {
			int indexReplace1 = rand() % binary_operators_1.size();
			std::string replaceStr1 = binary_operators_1[indexReplace1];
            
            int indexReplace2 = rand() % binary_operators_2.size();
            std::string replaceStr2 = binary_operators_2[indexReplace2];


			std::size_t pos1 = expr1.find(replaceStr1); 
			std::size_t pos2 = expr2.find(replaceStr2);

            std::string term1a = expr1.substr(0, pos1);
            std::string term1b = expr1.substr(pos1 + 1);
                
            std::string term2a = expr2.substr(0, pos2);
            std::string term2b = expr2.substr(pos2 + 1);

            //std::cout << "\n" << std::endl;

            if (term1a.back() == '(' || term2a.back() == '(') {
                replaceStr0 = ' -';
            }
            
            // with 50% change
            if ((double)rand() / (RAND_MAX) < 0.5) {



                child = term1a + replaceStr0 + term2b;
                // std::cout << "term1a: " << term1a << " ";
                // std::cout << "term2b: " << term2b << " ";
                // std::cout << "operator: " << replaceStr0 << " ";
                // std::cout << std::endl;
            }
            else {


                child = term2a + replaceStr0 + term1b;
                // std::cout << "term2a: " << term2a << " ";
                // std::cout << "term1b: " << term1b << " ";
                // std::cout << "operator: " << replaceStr0 << " ";
                // std::cout << std::endl;
            }


            return balance_parent(child);

		}

        else {
            // with 50% of chance 
            if ((double)rand() / (RAND_MAX) < 0.5) {
				return expr1;
			}
            else {
				return expr2;
			}
        }
    }

    std::vector<std::string> cross_expressions(const std::vector<std::string>& elite_expressions) {
        std::vector<std::string> crossed_expressions;
        int loop_limit = elite_expressions.size() - 1;
        std::string cross_mini;

        for (int i = 0; i < loop_limit; i++) {
            // print the elite expressions
            
            // std::cout << "elite expression 1: " << elite_expressions[i] << " ";
            // std::cout << "elite expression 2: " << elite_expressions[i + 1] << " ";
            // std::cout << std::endl;

            std::string crossed_expr = this -> merge_expressions2(elite_expressions[i], elite_expressions[i + 1]);
            
            
            // std::cout << " crossed expression " << crossed_expr << std::endl;
            
            cross_mini = combineLikeTerms(crossed_expr);
            
            // std::cout << " crossed expression mini" << cross_mini << std::endl;

            // crossed_expressions.push_back(crossed_expr);
            crossed_expressions.push_back(cross_mini);

        }

        return crossed_expressions;

    }

    std::vector<std::string> get_new_population(std::vector<std::string>& crossed_expressions, std::vector<std::string>& elite_expressions, int population_size, int depth) {

        std::vector<std::string> new_expressions;
        new_expressions.reserve(population_size);

        for (auto& elite_expr : elite_expressions) {
            new_expressions.emplace_back(std::move(elite_expr));
        }

        for (auto& crossed_expr : crossed_expressions) {
            new_expressions.emplace_back(std::move(crossed_expr));
        }

        while (new_expressions.size() < population_size) {
            new_expressions.emplace_back(generate_random_expr(depth));
        }

        return new_expressions;
    }

    std::vector<std::pair<double, std::string>> sorted_expressions(std::vector<std::string> expressions, std::string metric) {

        std::vector<std::pair<double, std::string>> mse_and_expression; // mse and expressions
        std::vector<std::pair<double, std::string>> elite_expressions;
        double mse_score;

        for (const auto& expr : expressions) {
            // std::cout << "Expression: " << expr << " // expr_size" << expr.size() << " ";
            // auto start1 = std::chrono::high_resolution_clock::now();

            std::vector<double> scores = this -> evaluate_fx_RPN2(expr);

            // auto stop1 = std::chrono::high_resolution_clock::now();
            // auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
            // std::cout << "scores: " << duration1.count() << " microseconds" << std::endl;

            // auto start2 = std::chrono::high_resolution_clock::now();

            mse_score = mse(y_vector, scores);

            // auto stop2 = std::chrono::high_resolution_clock::now();
            // auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);
            // std::cout << "mse: " << duration2.count() << " microseconds" << std::endl;

            mse_and_expression.emplace_back(mse_score, expr);
            // std::cout << "mse: " << mse_score << std::endl;
        }
        std::sort(mse_and_expression.begin(), mse_and_expression.end());
        //std::cout << " sorted " << std::endl;
        return mse_and_expression;

    }

    std::string add_term_left(std::string expr) {
        // Generate a new term with depth 1 (either a unary operation or a simple binary operation)
        std::string new_term = generate_random_expr(1);

        // Add the new term to the existing expression using a random operator
        std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];

        return "(" + new_term + " " + op + " " + expr + ")";
    }

    std::string add_term_right(std::string expr) {
        // Generate a new term with depth 1 (either a unary operation or a simple binary operation)
        std::string new_term = generate_random_expr(1);

        // Add the new term to the existing expression using a random operator
        std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];

        return "(" + expr + " " + op + " " + new_term + ")";
    }

    std::string remove_term_left(std::string expr) {
        std::size_t first_space = expr.find(' ');

        // If there is no space, then the expression is just a single term, and we return an empty string
        if (first_space == std::string::npos) {
            return "";
        }

        // Find the next space to identify the boundary of the term to remove
        std::size_t second_space = expr.find(' ', first_space + 1);

        // If there is no second space, that means there is only one operator left; remove it along with the preceding term
        if (second_space == std::string::npos) {
            return expr.substr(first_space + 1);
        }

        // Otherwise, remove the first term along with the following operator
        return expr.substr(second_space + 1);
    }

    std::string remove_term_right(std::string expr) {
        std::size_t last_space = expr.rfind(' ');

        // If there is no space, then the expression is just a single term, and we return an empty string
        if (last_space == std::string::npos) {
            return "";
        }

        // Find the second to last space to identify the boundary of the term to remove
        std::size_t second_last_space = expr.rfind(' ', last_space - 1);

        // If there is no second to last space, that means there is only one operator left; remove it along with the succeeding term
        if (second_last_space == std::string::npos) {
            return expr.substr(0, last_space);
        }

        // Otherwise, remove the last term along with the preceding operator
        return expr.substr(0, second_last_space);
    }

    std::string modify_expression(std::string expr) {

        // std::cout << "Original expression " << expr << " ";

        int n_binary_operators = binary_operators.size();
        int n_unary_operators = unary_operators.size();
        int n_terminals = terminals.size();
        int n_constants = constants.size();

        int indexToSearch;
        int indexReplace;

        std::string toSearch;
        std::string replaceStr;
        std::size_t pos;
        std::size_t lenString;

        //////////////////////
        // binary_operators //
        //////////////////////


        indexToSearch = rand() % n_binary_operators;
        toSearch = binary_operators[indexToSearch];

        indexReplace = rand() % n_binary_operators;
        replaceStr = binary_operators[indexReplace];

        lenString = toSearch.length();

        pos = expr.find(toSearch);

        if (pos != std::string::npos) {
            // Check for special condition only if pos is greater than 0
            if (pos > 0 && expr[pos] == '-' && expr[pos - 1] == '(') {
                replaceStr = "-";
            }
            // The number of characters to replace could be the length of 'toSearch'
            // but we're safely using lenString
            expr.replace(pos, lenString, replaceStr);
        }
        //////////////////////
        // unary_operators //
        //////////////////////

        if(n_unary_operators > 0){
            indexToSearch = rand() % n_unary_operators;
            toSearch = unary_operators[indexToSearch];

            indexReplace = rand() % n_unary_operators;
            replaceStr = unary_operators[indexReplace];

            lenString = toSearch.length();

            pos = expr.find(toSearch);
            if (pos != std::string::npos) {
                // Replace it with "cos" function
                expr.replace(pos, lenString, replaceStr);
            }

        }
        ///////////////////////////
        // terminals            //
        //////////////////////////


        indexToSearch = std::rand() % n_terminals;
        toSearch = terminals[indexToSearch];
        
        indexReplace = std::rand() % n_terminals;
        replaceStr = terminals[indexReplace];

        lenString = toSearch.length();

        pos = expr.find(toSearch);
        if (pos != std::string::npos) {
            expr.replace(pos, lenString, replaceStr);
        }


        ///////////////////////////
        // constants            //
        //////////////////////////


        indexToSearch = std::rand() % n_constants;
        toSearch = constants[indexToSearch];

        indexReplace = std::rand() % n_constants;
        replaceStr = constants[indexReplace];

        // len of replaceStr
        lenString = toSearch.length();

        pos = expr.find(toSearch);
        if (pos != std::string::npos) {
            expr.replace(pos, lenString, replaceStr); //
        }

        ///////////////////////////

        // std::cout << " Cambiada por " << expr << std::endl;

        return expr;
    }

    std::vector<std::string> mutation2(const std::vector<std::string>& expressions, double mutation_prob, double elite_perc, double grow_prob, double prune_prob) {

        int lenUnary = unary_operators.size();
        double prune_size = lenUnary > 0 ? 8 : 5;

        int population_size = expressions.size();
        std::vector<std::string> new_expressions;
        new_expressions.reserve(population_size);
        std::string mutated_expr;

        // Do not touch elite expressions
        size_t elite_count = static_cast<size_t>(std::ceil(elite_perc * expressions.size()));
        for (size_t i = 0; i < elite_count; ++i) {
            new_expressions.emplace_back(expressions[i]);
        }

        // Apply mutation with a certain probability
        for (size_t i = elite_count; i < population_size; ++i) {
            mutated_expr = expressions[i];

            // std::cout << "Before: " << mutated_expr << " ";

            double rand_mutation = ((double)rand() / (RAND_MAX));
            if (rand_mutation < mutation_prob) {
                mutated_expr = modify_expression(mutated_expr);
            }
            
            double rand_prune = ((double)rand() / (RAND_MAX));
            // if len ura > 0 size ...

            

            if (rand_prune < prune_prob && mutated_expr.size() > prune_size) {
                if (((double)rand() / (RAND_MAX)) < 0.5) {
                    mutated_expr = balance_parent(remove_term_left(mutated_expr));
                }
                else {
                    mutated_expr = balance_parent(remove_term_right(mutated_expr));
                }
            }

            double rand_grow = ((double)rand() / (RAND_MAX));
            if (rand_grow < grow_prob) {
                if (((double)rand() / (RAND_MAX)) < 0.5) {
                    mutated_expr = add_term_left(mutated_expr);
                }
                else {
                    mutated_expr = add_term_right(mutated_expr);
                }
            }
            // std::cout << "After: " << mutated_expr << std::endl;
            if (mutated_expr.size() > 0) {
                new_expressions.emplace_back(mutated_expr);
			}
            else {
                new_expressions.emplace_back(generate_random_expr(3));
            }
            
        }

        return new_expressions;
    }

    std::string replaceDoubleAsterisks(std::string str) {

        int depth = random_int(3, 10);

        if (str.find("zoo") != std::string::npos) {
            int depth = random_int(3, 10);
            return generate_random_expr(depth);
        }
        else{
            size_t pos = 0;
            while ((pos = str.find("**", pos)) != std::string::npos) {
                str.replace(pos, 2, " ^ "); // Replace 2 characters at pos with " ^ "
                pos += 3;  // Move past the replaced characters (" ^ ").
            }
            return "(" + str + ")";
        }
    }

    std::string combineLikeTerms(std::string infix_expression) {


        std::string simplified_expression_str;
        std::string ses2;
        try {
            // std::cout << "infix_expression: " << infix_expression << " ";
            py::object simplified_expression = sympify(infix_expression);
            // std::cout << "simplified_expression: " << py::str(simplified_expression) << " ";
            py::object expanded_expression = simplified_expression.attr("expand")();
            simplified_expression_str = py::str(expanded_expression);
            // std::cout << "simplified_expression_str: " << simplified_expression_str << std::endl;
            ses2 = replaceDoubleAsterisks(simplified_expression_str);
        }
        catch (py::error_already_set& e) {
            

            int depth = random_int(3,10);
            ses2 = generate_random_expr(depth);

        }

        return py::str(ses2);
    }

    std::string expander(std::string infix_expression) {
        std::string simplified_expression_str;
        std::string expanded_expression_str;
        std::string ses1;
        std::string ses2;

        try {
            py::object simplified_expression = sympify(infix_expression);
            py::object expanded_expression = simplified_expression.attr("expand")();  
            simplified_expression_str = py::str(expanded_expression);
            ses2 = replaceDoubleAsterisks(simplified_expression_str);

        }
        catch (py::error_already_set& e) {
            int depth = random_int(3, 10);
            ses2 = generate_random_expr(depth);
        }

        return ses2;  // Changed from py::str(ses2) to just ses2
    }

    void clearVectors(std::vector<std::string>& elite, std::vector<std::string>& cross_elite,
        std::vector<std::string>& new_population, std::vector<std::string>& mutated_new_population) {
        elite.clear();
        cross_elite.clear();
        new_population.clear();
        mutated_new_population.clear();
    }

    
    void elite_viewer(std::vector<std::pair<double, std::string>> sorted_expressions_vec, int gen) {
        if (!sorted_expressions_vec.empty()) { // Checking that the vector is not empty
            auto first_element = sorted_expressions_vec.front(); // Getting the first element
            std::cout <<"Generation: " << gen <<" First element: [" << first_element.first << ", " << first_element.second << "]" << std::endl;            
        }
        else {
            std::cout << "The vector is empty." << std::endl;
        }
    }

    void remove_test(std::vector<std::pair<double, std::string>> sorted_expressions_vec) {
		if (!sorted_expressions_vec.empty()) { // Checking that the vector is not empty
        			auto first_element = sorted_expressions_vec.front(); // Getting the first element
                    std::string ty = remove_term_left(first_element.second);
                    std::string ty2 = remove_term_right(first_element.second);
                    std::cout << "Remove term left: " << balance_parent(ty) << std::endl;
                    std::cout << "Remove term right: " << balance_parent(ty2) << std::endl;
        		}
        else {
			std::cout << "The vector is empty." << std::endl;
		}
	}

    void expander_viewer(std::vector<std::pair<double, std::string>> sorted_expressions_vec) {
		if (!sorted_expressions_vec.empty()) { // Checking that the vector is not empty
        			auto first_element = sorted_expressions_vec.front(); // Getting the first element
                    std::string ty = expander(first_element.second);
                    std::cout << "Expanded: " << balance_parent(ty) << std::endl;
        		}
        else {
			std::cout << "The vector is empty." << std::endl;
		}
	}

    std::vector<std::pair<double, std::string>> genetic_training(int population_size, int depth, int generations, std::string metric, double elite_perc,
        double mutation_prob, double grow_prob, double prune_prob, double early_stop, bool verbose) {

        std::vector<std::string> expressions = this->create_initial_population(population_size, depth);
        std::vector<std::pair<double, std::string>> sorted_expressions_vec;
        std::vector<std::string> elite;
        std::vector<std::string> cross_elite;
        std::vector<std::string> new_population;
        std::vector<std::string> mutated_new_population;
        
        double best_mse;


        for (int gen = 0; gen < generations; gen++) {
           
            std::cout << "sorted_expressions_vec" << " ";
            auto start1 = std::chrono::high_resolution_clock::now();
            sorted_expressions_vec = this->sorted_expressions(expressions, metric);
            auto stop1 = std::chrono::high_resolution_clock::now();
            auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(stop1 - start1);
            std::cout << "sorted_expressions: " << duration1.count() << " microseconds" << std::endl;

            std::cout << "get_elite" << " ";
            auto start2 = std::chrono::high_resolution_clock::now();
            elite = this -> get_elite(sorted_expressions_vec, elite_perc);
            auto stop2 = std::chrono::high_resolution_clock::now();
            auto duration2 = std::chrono::duration_cast<std::chrono::microseconds>(stop2 - start2);
            std::cout << "get_elite: " << duration2.count() << " microseconds" << std::endl;

            std::cout << "cross_expressions" << " ";
            auto start3 = std::chrono::high_resolution_clock::now();
            cross_elite = this -> cross_expressions(elite);
            auto stop3 = std::chrono::high_resolution_clock::now();
            auto duration3 = std::chrono::duration_cast<std::chrono::microseconds>(stop3 - start3);
            std::cout << "cross_expressions: " << duration3.count() << " microseconds" << std::endl;

            std::cout << "get_new_population" << " ";
            auto start4 = std::chrono::high_resolution_clock::now();
            new_population = this -> get_new_population(elite, cross_elite, population_size, depth);
            auto stop4 = std::chrono::high_resolution_clock::now();
            auto duration4 = std::chrono::duration_cast<std::chrono::microseconds>(stop4 - start4);
            std::cout << "get_new_population: " << duration4.count() << " microseconds" << std::endl;

            std::cout << "mutation2" << " ";
            auto start5 = std::chrono::high_resolution_clock::now();
            mutated_new_population = this -> mutation2(new_population, mutation_prob, elite_perc, grow_prob, prune_prob);
            auto stop5 = std::chrono::high_resolution_clock::now();
            auto duration5 = std::chrono::duration_cast<std::chrono::microseconds>(stop5 - start5);
            std::cout << "mutation2: " << duration5.count() << " microseconds" << std::endl;


            expressions = std::move(mutated_new_population);

            elite_viewer(sorted_expressions_vec, gen);

            // remove_test(sorted_expressions_vec);

            // expander_viewer(sorted_expressions_vec);

            
            if (!sorted_expressions_vec.empty() && sorted_expressions_vec.front().first <= early_stop) { // If the best_mse (First element) is < than n break.
                break;
            }


            clearVectors(elite, cross_elite, new_population, mutated_new_population);

        }
        std::vector<std::pair<double, std::string>> final_expression_vec = sorted_expressions(expressions, metric);

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
        .def("evaluationArray2", &Training::evaluationArray2, "function that evaluates the RPN")
        .def("evaluate_fx_RPN2", &Training::evaluate_fx_RPN2, "function that evaluates the RPN")
        .def("sorted_expressions", &Training::sorted_expressions, "function that sorts the expressions")
        .def("get_elite", &Training::get_elite, "function that gets the elite")
        .def("cross_expressions", &Training::cross_expressions, "function that crosses the expressions")
        .def("mutation2", &Training::mutation2, "function that mutates the expressions")
        .def("get_new_population", &Training::get_new_population, "function that gets the new population")
        .def("generate_random_expr", &Training::generate_random_expr, "function that generates a random expression")
        .def("create_initial_population", &Training::create_initial_population, "function that creates the initial population")
        .def("genetic_training", &Training::genetic_training, "function that runs the training of the symbolic regression using genetic training");
}