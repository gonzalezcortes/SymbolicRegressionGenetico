#include <iostream>
#include <stack>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <regex>



int precedence2(const std::string& op) {
    if (op == "+" || op == "-")
        return 1;
    if (op == "*" || op == "/")
        return 2;
    if (op == "^" || op == "sin" || op == "cos" || op == "exp")
        return 3;
    return -1;
}

std::vector<std::string> infixToRPN2(std::istringstream& infix) {
    std::stack<std::string> operators;
    std::vector<std::string> rpn;
    std::string token;

    while (infix >> token) {
        if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^" || token == "sin" || token == "cos" || token == "exp") {
            while (!operators.empty() && precedence2(operators.top()) >= precedence2(token) && operators.top() != "(") {
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
            if (!operators.empty()) {
                operators.pop(); // Pop the '(' symbol.
            }
        } else {
            rpn.push_back(token);
        }
    }

    while (!operators.empty()) {
        rpn.push_back(operators.top());
        operators.pop();
    }

    return rpn;
}













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
        std::cout << token << std::endl;
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



int main() {
    std::string rpn_expression;
    //std::cout << "Enter a Reverse Polish Notation expression: ";
    //std::getline(std::cin, rpn_expression);

    //std::istringstream expression_stream(rpn_expression);
    //double result = evaluateRPN2(expression_stream);
    //std::cout << "Result: " << result << std::endl;

    // infix
    std::string infix_expression;
    std::cout << "Enter an infix expression: ";
    std::getline(std::cin, infix_expression);



    // la solucion no funciona
    std::string spaced_expression = addSpacesToInfix(infix_expression);
    spaced_expression = std::regex_replace(spaced_expression, std::regex("\\-\\s*\\d"), "0 $&");  // $& refers to the entire matched expression

    std::cout << spaced_expression << std::endl;

    // std::string spaced_expression = addSpacesToInfix(infix_expression);
    std::istringstream expression_stream(spaced_expression);

    // std::istringstream expression_stream(infix_expression);

    std::vector<std::string> rpn = infixToRPN2(expression_stream);

    std::cout << "RPN expression: ";
    for (const auto &token : rpn) {
        std::cout << token << " ";
    };

    std::cout << "\n" << std::endl;

    double result = evaluateRPN2(rpn);

    std::cout << "Result: " << result << std::endl;


    return 0;

}
