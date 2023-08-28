#include <iostream>
#include <stack>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>



int precedence2(const std::string& op) {
    if (op == "+" || op == "-")
        return 1;
    if (op == "*" || op == "/")
        return 2;
    if (op == "^" || op == "sin" || op == "cos" || op == "exp")
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
        } else {
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
        } else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            while (!operators.empty() && precedence2(operators.top()) >= precedence2(token)) {
                rpn.push_back(operators.top());
                operators.pop();
            }
            operators.push(token);
        } else if (token == "(") {
            operators.push(token);
        } else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                rpn.push_back(operators.top());
                operators.pop();
            }
            operators.pop();
        } else {
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


// In reverse Polish notation, the operators follow their operands.
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
                    std::cerr << "Division by zero\n";
                    return -1;
                }
                result = a / b;
            } else if (token == "^") result = std::pow(a, b);
            values.push(result);
        } else if (token == "u-") {
            double a = values.top(); values.pop();
            values.push(-a);
        } else {
            values.push(std::stod(token));
        }
    }
    return values.top();
}

std::string rpnToInfix(const std::vector<std::string>& rpn) {
    std::stack<std::string> stack;
    for (const auto& token : rpn) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            std::string op2 = stack.top(); stack.pop();
            std::string op1 = stack.top(); stack.pop();
            std::string expr = "(" + op1 + " " + token + " " + op2 + ")";
            stack.push(expr);
        }
        else if (token == "sin" || token == "cos" || token == "exp") {
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

double perform_simplification(double a, double b, const std::string& op) {

    if (op == "+") {
        return a + b;
    } else if (op == "*") {
        return a * b;
    } else if (op == "-"){
        return a - b;
    } else if (op == "/"){
        return a/b;
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
    } catch (const std::invalid_argument&) {
        return false;
    }
}


std::string rpnToInfix2(const std::vector<std::string>& rpn) {
    std::stack<std::string> stack;
    double intermedia;
    for (const auto& token : rpn) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            std::string op2 = stack.top(); stack.pop();
            std::string op1 = stack.top(); stack.pop();

            if (isConvertibleToDouble(op1) && isConvertibleToDouble(op2)) {
                double op1_double = std::stod(op1);
                double op2_double = std::stod(op2);
                std::cout << "op1_double" << op1_double << std::endl;
                std::cout << "op2_double" << op2_double << std::endl;

                intermedia = perform_simplification(op1_double, op2_double, token);
                std::string expr = "(" + std::to_string(intermedia) + ")";
                stack.push(expr);
            }
            else{
                std::string expr = "(" + op1 + token + op2 + ")";
                stack.push(expr);
            }
        }
        else if (token == "sin" || token == "cos" || token == "exp") {
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



std::string addSpacesToInfix(const std::string& infix) {
    std::string spacedInfix;
    for (char ch : infix) {
        if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '(' || ch == ')') {
            spacedInfix += ' ';
            spacedInfix += ch;
            spacedInfix += ' ';
        } else {
            spacedInfix += ch;
        }
    }
    return spacedInfix;
}

std::string removeSpacesFromExpression(const std::string& expression) {
    std::string result;
    for (char ch : expression) {
        if (ch != ' ') {
            result += ch;
        }
    }
    return result;
}


int main() {
    std::string infix;
    std::cout << "Enter an infix expression: ";
    std::getline(std::cin, infix);

    infix = addSpaces(infix);
    std::istringstream iss(infix);
    auto rpn = infixToRPN2(iss);

    std::cout << "Reverse Polish Notation: ";
    for (const auto& t : rpn) std::cout << t << ' ';
    std::cout << '\n';

    double result = evaluateRPN2(rpn);
    std::cout << "Result: " << result << '\n';

    return 0;
}
