#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include <stack>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>

#include <unordered_map> // evaluateRPN3
#include <functional> // evaluateRPN3


std::string rpnToInfix(const std::vector<std::string>& rpn) {
    std::stack<std::string> stack;
    for (const auto& token : rpn) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            std::string op2 = stack.top(); stack.pop();
            std::string op1 = stack.top(); stack.pop();
            std::string expr = "(" + op1 + " " + token + " " + op2 + ")";
            stack.push(expr);
        }
        else if (token == "sin" || token == "cos" || token == "exp" || token == "sqrt") {
            std::string op = stack.top(); stack.pop();
            std::string expr = token + "(" + op + ")";
            stack.push(expr);
        }
        else {
            stack.push(token);
        }
    }
    return stack.top();
}


int precedence2(const std::string& op) {
    if (op == "+" || op == "-")
        return 1;
    if (op == "^" || op == "*" || op == "/")
        return 2;
    if (op == "sin" || op == "cos" || op == "exp" )
        return 3;
    if (op == "u-")  // unary minus
        return 4;
    if (op == "sqrt")  // unary minus
        return 5;
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

std::vector<std::string> infixToRPN3(std::istringstream& infix) {
    std::stack<std::string> operators;
    std::vector<std::string> rpn;
    std::string token;
    std::string lastToken = "("; // Initialize to left parenthesis for unary negation checks

    while (infix >> token) {
        if (token == "-" && (lastToken == "(" || precedence2(lastToken) > 0)) {
            operators.push("-");
            rpn.push_back("0"); // Push 0 to handle unary negation
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^" || token == "sin" || token == "cos" || token == "exp" || token == "sqrt") {
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


double evaluateRPN3(const std::vector<std::string>& rpn) {
    std::stack<double> values;
    std::map<std::string, std::function<double(double, double)>> ops = {
        {"+", [](double a, double b) { return a + b; }},
        {"-", [](double a, double b) { return a - b; }},
        {"*", [](double a, double b) { return a * b; }},
        {"/", [](double a, double b) { return b != 0 ? a / b : -1; }},  // Returning -1 in case of division by zero
        {"^", [](double a, double b) { return std::pow(a, b); }}
    };

    for (const auto& token : rpn) {
        if (ops.find(token) != ops.end()) {
            if (values.size() < 2) return -1;  // Error code for insufficient operands
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            double result = ops[token](a, b);
            if (token == "/" && b == 0) return -1;  // Check for division by zero result
            values.push(result);
        }
        else if (token == "u-") {
            if (values.empty()) return -1;  // Error code for stack underflow
            double a = values.top(); values.pop();
            values.push(-a);
        }
        else {
            values.push(std::stod(token));
        }
    }

    if (values.size() != 1) return -1;  // Error code for invalid expression
    return values.top();
}

double evaluateRPN2B(const std::vector<std::string>& rpn) {
    // it is not faster than evaluateRPN2
    std::stack<double> values;

    std::map<std::string, std::function<double(double, double)>> ops = {
        {"+", [](double a, double b) { return a + b; }},
        {"-", [](double a, double b) { return a - b; }},
        {"*", [](double a, double b) { return a * b; }},
        {"/", [](double a, double b) { return a / b; }},
        {"^", [](double a, double b) { return std::pow(a, b); }}
    };

    std::map<std::string, std::function<double(double)>> ops_single = {
        {"u-", [](double operand) { return -operand; }},
        {"sin", [](double operand) { return sin(operand); }},
		{"cos", [](double operand) { return cos(operand); }},
		{"exp", [](double operand) { return exp(operand); }},
		{"sqrt", [](double operand) { return sqrt(operand); }}
    };

    for (const auto& token : rpn) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            double b = values.top(); values.pop();
            double a = values.top(); values.pop();
            double result = 0;
            if (token == "/" && b == 0) return -1;
            result = ops[token](a, b);
            values.push(result);
        }
        else if (token == "u-" || token == "sin" || token == "cos" || token == "exp" || token == "sqrt") {
            double operand = values.top(); values.pop();
            double result1 = ops_single[token](operand);
            values.push(result1);
        }
        else {
            values.push(std::stod(token));
        }
    }
    return values.top();
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
        
        else if (token == "sqrt") {
			double operand = values.top(); values.pop();
			values.push(sqrt(operand));
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

bool isConvertibleToDouble(const std::string& str) {
    try {
        std::stod(str);
        return true;
    }
    catch (const std::invalid_argument&) {
        return false;
    }
}






namespace py = pybind11;

PYBIND11_MODULE(reverseNotation, m) {
    m.def("precedence2", &precedence2, "A function that do reverse Notation");
    m.def("addSpaces", &addSpaces, "A function that do reverse Notation");

    m.def("infixToRPN3", &infixToRPN3, "A function that do reverse Notation");
    m.def("evaluateRPN2", &evaluateRPN2, "A function that do reverse Notation");

    m.def("perform_simplification", &perform_simplification, "A function that do reverse Notation");
    m.def("isConvertibleToDouble", &isConvertibleToDouble, "A function that do reverse Notation");

    m.def("rpnToInfix", &rpnToInfix, "rpnToInfix");
}
