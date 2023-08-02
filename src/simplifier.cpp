#include <iostream>
#include <stack>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <vector>

int precedence(char op) {
    if (op == '+' || op == '-')
        return 1;
    if (op == '*' || op == '/')
        return 2;
    if (op == '^')
        return 3;
    return -1;
}

std::vector<std::string> infixToRPN(std::istringstream& infix) {
    std::stack<char> operators;
    std::vector<std::string> rpn;
    std::string token;

    while (infix >> token) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
            while (!operators.empty() && precedence(operators.top()) >= precedence(token[0]) && operators.top() != '(') {
                rpn.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            operators.push(token[0]);
        } else if (token == "(") {
            operators.push(token[0]);
        } else if (token == ")") {
            while (!operators.empty() && operators.top() != '(') {
                rpn.push_back(std::string(1, operators.top()));
                operators.pop();
            }
            if (!operators.empty()) {
                operators.pop(); // Pop the '(' symbol.
            }
        } else {
            rpn.push_back(token);
        }
    }

    while (!operators.empty()) {
        rpn.push_back(std::string(1, operators.top()));
        operators.pop();
    }

    return rpn;
}

// In reverse Polish notation, the operators follow their operands.
double evaluateRPN2(std::vector<std::string>& rpn) {
    std::stack<double> values;

    for (const auto& token : rpn) {

        if (token == "+" || token == "-" || token == "*" || token == "/") {
            double right_operand = values.top(); values.pop();
            double left_operand = values.top(); values.pop();

            if (token == "+") {
                values.push(left_operand + right_operand);
            }
            else if (token == "-") {
                values.push(left_operand - right_operand);
            }
            else if (token == "*") {
                values.push(left_operand * right_operand);
            }
            else if (token == "/") {
                if(right_operand != 0) {
                    values.push(left_operand / right_operand);
                } else {
                    std::cerr << "Error: Division by zero!" << std::endl;
                    return 0;
                }
            }
        } else if (token == "sin") {
            double operand = values.top(); values.pop();
            values.push(sin(operand));
        } else if (token == "cos") {
            double operand = values.top(); values.pop();
            values.push(cos(operand));
        } else if (token == "exp") {
            double operand = values.top(); values.pop();
            values.push(exp(operand));
        } else {
            values.push(std::stod(token));
        }
    }

    return values.top();
}

double evaluateRPN(std::vector<std::string>& rpn) {
    std::stack<double> values;

    for (const auto& token : rpn) {
        if (token == "+" || token == "-" || token == "*" || token == "/") {
            double right_operand = values.top(); values.pop();
            double left_operand = values.top(); values.pop();

            if (token == "+") {
                values.push(left_operand + right_operand);
            }
            else if (token == "-") {
                values.push(left_operand - right_operand);
            }
            else if (token == "*") {
                values.push(left_operand * right_operand);
            }
            else if (token == "/") {
                if(right_operand != 0) {
                    values.push(left_operand / right_operand);
                } else {
                    std::cerr << "Error: Division by zero!" << std::endl;
                    return 0;
                }
            }
        } else {
            values.push(std::stod(token));
        }
    }

    return values.top();
}

// create RPN expression



