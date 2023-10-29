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

        lenUnary = unary_operators.size();

        if (lenUnary > 0) {
            unary_prob = 0.1;
        }
        else {
            unary_prob = 0;
        }

        if (depth > 0 && ((double)rand() / (RAND_MAX)) < 0.9) {
            // 50% chance to choose unary or binary operator
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
            }
            */
            
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

        // std::cout << "infix RPN" << std::endl;
        std::vector<std::string> rpn = infixToRPN3(iss); // convert the expression to RPN

        //print the RPN
        /*
        for (const auto& token : rpn) {
			std::cout << token << ' ';
		}
        std::cout << std::endl;
        */
        
        // std::cout << "Evaluation Array" << std::endl;



        std::vector<double> evaluation_vector = this -> evaluationArray2(rpn);

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

            // count the '(' and ')' in the child
            int count_open = std::count(child.begin(), child.end(), '(');
            int count_close = std::count(child.begin(), child.end(), ')');
            // std::cout << "count open " << count_open << " count close " << count_close << std::endl;

            // Fill the missing '(' or ')' in the child
            if (count_open > count_close) {
                // Too many opening parentheses, add closing ones
                for (int i = 0; i < (count_open - count_close); ++i) {
                    child += ')';
                }
            }
            else if (count_close > count_open) {
                // Too many closing parentheses, add opening ones at the beginning
                for (int i = 0; i < (count_close - count_open); ++i) {
                    child = '(' + child;
                }
            }

            return child;

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

        std::vector<std::pair<double, std::string>> mse_and_expression; // mse and expressions
        std::vector<std::pair<double, std::string>> elite_expressions;
        double mse_score;

        for (const auto& expr : expressions) {
            // std::cout << "Expression: " << expr << " ";
            std::vector<double> scores = this -> evaluate_fx_RPN2(expr);
            mse_score = mse(y_vector, scores);
            mse_and_expression.emplace_back(mse_score, expr);
            // std::cout << "mse: " << mse_score << std::endl;
        }

        std::sort(mse_and_expression.begin(), mse_and_expression.end());

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
            py::object sympify = py::module::import("sympy").attr("sympify");
            py::object simplified_expression = sympify(infix_expression);
            simplified_expression_str = py::str(simplified_expression);
            ses2 = replaceDoubleAsterisks(simplified_expression_str);
        }
        catch (py::error_already_set& e) {
            

            int depth = random_int(3,10);
            ses2 = generate_random_expr(depth);

        }

        return py::str(ses2);
    }

    /*
    void early_stop(std::vector<std::pair<double, std::string>> sorted_expressions_vec, int gen) {
        if (!sorted_expressions_vec.empty()) { // Checking that the vector is not empty
            auto first_element = sorted_expressions_vec.front(); // Getting the first element
            std::cout <<"Generation: " << gen <<" First element: (" << first_element.first << ", " << first_element.second << ")" << std::endl;
            std::cout << first_element.first << std::endl;
            if (first_element.first < 0.01) {
                std::cout << "Break " << std::endl;
                double first_elementA = sorted_expressions_vec.front().first;
                std::cout << first_elementA  << std::endl;

            }
            
        }
        else {
            std::cout << "The vector is empty." << std::endl;
        }
    }
    */

    std::vector<std::pair<double, std::string>> genetic_training(int population_size, int depth, int generations, std::string metric, double elite_perc,
        double mutation_prob, double grow_prob, double early_stop, bool verbose) {

        std::vector<std::string> expressions = this->create_initial_population(population_size, depth);
        std::vector<std::pair<double, std::string>> sorted_expressions_vec;
        std::vector<std::string> elite;
        std::vector<std::string> cross_elite;
        std::vector<std::string> new_population;
        std::vector<std::string> mutated_new_population;
        
        double best_mse;


        for (int gen = 0; gen < generations; gen++) {
            if(verbose==true){
                std::cout << "Generation: " << gen << std::endl;
                for (auto expr_d : expressions) {
                    std::cout << expr_d << std::endl;
                }
            }
            
            sorted_expressions_vec = this->sorted_expressions(expressions, metric);

            elite = this -> get_elite(sorted_expressions_vec, elite_perc); 

            cross_elite = this -> cross_expressions(elite);
            
            new_population = this -> get_new_population(elite, cross_elite, population_size, depth);

            mutated_new_population = this -> mutation(new_population, mutation_prob, elite_perc, grow_prob);

            expressions = mutated_new_population;

            best_mse = sorted_expressions_vec.front().first;

            if (best_mse <= early_stop) {
                break;
            }

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
        .def("mutation", &Training::mutation, "function that mutates the expressions")
        .def("get_new_population", &Training::get_new_population, "function that gets the new population")
        .def("generate_random_expr", &Training::generate_random_expr, "function that generates a random expression")
        .def("create_initial_population", &Training::create_initial_population, "function that creates the initial population")
        .def("genetic_training", &Training::genetic_training, "function that runs the training of the symbolic regression using genetic training");
}