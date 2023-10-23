#include <iostream>
#include <stack>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <map>


std::vector<std::string> binary_operators = { "+", "-", "*", "/" };
std::vector<std::string> unary_operators = { "sin", "cos" , "exp" };
std::vector<std::string> terminals = { "X" };
std::vector<std::string> constants = { "1", "2", "3" };

bool isConvertibleToDouble(const std::string& str) {
    try {
        std::stod(str);
        return true;
    }
    catch (const std::invalid_argument&) {
        return false;
    }
}

std::string rpnToInfix(const std::vector<std::string>& rpn) {
    std::stack<std::string> stack;

    for (const auto& token : rpn) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            std::string op2 = stack.top(); stack.pop();
            std::string op1 = stack.top(); stack.pop();
            if (isConvertibleToDouble(op1) && isConvertibleToDouble(op2)) {
                // Perform arithmetic if both operands are numbers
                double a = std::stod(op1);
                double b = std::stod(op2);
                double result = 0;
                if (token == "+") result = a + b;
                else if (token == "-") result = a - b;
                else if (token == "*") result = a * b;
                else if (token == "/") result = a / b;
                else if (token == "^") result = std::pow(a, b);
                stack.push(std::to_string(result));
            }
            else {
                // Otherwise, keep it as an expression
                std::string expr = "(" + op1 + " " + token + " " + op2 + ")";
                stack.push(expr);
            }
        }
        else {
            stack.push(token);
        }
    }

    return stack.top();
}


int random_int(int min, int max) {
    return min + rand() % (max - min + 1);
}

int precedence2(const std::string& op) {
    if (op == "+" || op == "-")
        return 1;
    if (op == "^" || op == "*" || op == "/")
        return 2;
    if (op == "sin" || op == "cos" || op == "exp")
        return 3;
    if (op == "u-")  // unary minus
        return 4;
    return -1;
}

std::string addSpaces(const std::string& str) {
    std::string spaced;
    for (char ch : str) {
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '(' || ch == ')') {
            spaced += ' ';
            spaced += ch;
            spaced += ' ';
        }
        else {
            spaced += ch;
        }
    }
    return spaced;
}

std::vector<std::string> infixToRPN2(std::istringstream& infix) {
    std::stack<std::string> operators;
    std::vector<std::string> rpn;
    std::string token;
    std::string lastToken = "("; // Initialize to left parenthesis for unary negation checks

    while (infix >> token) {
        if (token == "-" && (lastToken == "(" || precedence2(lastToken) > 0)) {
            operators.push("u-");
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^" || token == "sin" || token == "cos" || token == "exp") {
            while (!operators.empty() && precedence2(operators.top()) >= precedence2(token)) {
                rpn.push_back(operators.top());
                operators.pop();
            }
            operators.push(token);
        }
        else if (token == "(") {
            operators.push(token);
        }
        else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                rpn.push_back(operators.top());
                operators.pop();
            }
            operators.pop();
        }
        else {
            rpn.push_back(token);
        }
        lastToken = token;
    }

    while (!operators.empty()) {
        rpn.push_back(operators.top());
        operators.pop();
    }

    return rpn;
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

double perform_simplification(double a, double b, const std::string& op) {

    if (op == "+") {
        return a + b;
    }
    else if (op == "*") {
        return a * b;
    }
    else if (op == "-") {
        return a - b;
    }
    else if (op == "/") {
        return a / b;
    }

    else {
        std::cerr << "Invalid operator" << std::endl;
        return 0;
    }
}


std::vector<std::string> reduceRPN(const std::vector<std::string>& rpn) {
    std::stack<std::string> st;
    double constantSum = 0;

    for (const auto& token : rpn) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            if (isConvertibleToDouble(st.top())) {
                double operand2 = std::stod(st.top()); st.pop();
                if (isConvertibleToDouble(st.top())) {
                    double operand1 = std::stod(st.top()); st.pop();
                    double result = perform_simplification(operand1, operand2, token);
                    st.push(std::to_string(result));
                }
                else {
                    std::string operand1 = st.top(); st.pop();
                    st.push(operand1 + " " + token + " " + std::to_string(operand2));
                }
            }
        }
        else if (token == "sin" || token == "cos" || token == "exp") {
            if (isConvertibleToDouble(st.top())) {
                double operand = std::stod(st.top()); st.pop();
                double result = 0;
                if (token == "sin") result = sin(operand);
                else if (token == "cos") result = cos(operand);
                else if (token == "exp") result = exp(operand);
                st.push(std::to_string(result));
            }
        }
        else if (isConvertibleToDouble(token)) {
            constantSum += std::stod(token);
        }
        else {
            st.push(token);
        }
    }

    if (constantSum != 0) {
        st.push(std::to_string(constantSum));
    }

    std::vector<std::string> reducedRPN;
    while (!st.empty()) {
        reducedRPN.insert(reducedRPN.begin(), st.top());
        st.pop();
    }

    return reducedRPN;
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


int main() {

    int population_size = 10;

    int depth = 3;

    std::vector<std::string> expressions = create_initial_population(population_size, depth);

    //print expressions 
    for (int i = 0; i < expressions.size(); i++) {
		std::cout << expressions[i] << std::endl;
	}

    std::vector<std::string> rpn;
    std::vector<std::string> rpny;
    std::vector<std::string> redp1;

    std::string infix_str = addSpaces("X*5+5+1+2-568+X+3+9+5+6+9");
    std::istringstream iss(infix_str);
    std::cout << "Initial expression: " << infix_str << std::endl;

    rpn = infixToRPN2(iss);
    std::cout << "RPN expression 1: ";
    for (const auto& token : rpn) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;

    std::string rp = rpnToInfix(rpn);
    std::cout << "Inflix outcome 1: " << rp << std::endl;


    std::string infix_str2 = addSpaces(rp);
    std::istringstream iss2(infix_str2);
    std::cout << "Infix income with spaces: " << infix_str2 << std::endl;

    rpny = infixToRPN2(iss2);
    std::cout << "RPN expression 2: ";
    for (const auto& token : rpny) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;

    std::string rpi = rpnToInfix(rpny);
    std::cout << "Inflix outcome 2: " << rpi << std::endl;

    std::cout << " Reduce RPN" << std::endl;
    redp1 = reduceRPN(rpn);
    //print redp1
    for (int i = 0; i < redp1.size(); i++) {
        std::cout << redp1[i] << " ";
	}
    std::cout << std::endl;



    return 0;
}