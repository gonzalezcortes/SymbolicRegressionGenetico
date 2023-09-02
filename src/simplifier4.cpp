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

std::string addSpaces2(const std::string& str) {
    std::string spaced;
    for (std::size_t i = 0; i < str.size(); ++i) {
        char ch = str[i];

        // Check for sin, cos, exp
        if (i + 2 < str.size() &&
            ((str.substr(i, 3) == "sin") ||
             (str.substr(i, 3) == "cos") ||
             (str.substr(i, 3) == "exp"))) {
            spaced += str.substr(i, 4);  // append the function name and opening parenthesis
            i += 3;  // skip ahead
        } else if (ch == '+' || ch == '-' || ch == '*' || ch == '/' || ch == '^' || ch == '(' || ch == ')') {
            spaced += ' ';
            spaced += ch;
            spaced += ' ';
        } else {
            spaced += ch;
        }
    }
    return spaced;
}

std::vector<std::string> infixToRPN(const std::string& infixStr) {
    std::istringstream infix(infixStr); // Create a stream from the string
    std::stack<std::string> operators;
    std::vector<std::string> rpn;
    std::string token;
    std::string lastToken = "("; // Initialize to left parenthesis for unary negation checks

    while (infix >> token) {
        if (token == "-" && (lastToken == "(" || precedence2(lastToken) > 0)) {
            operators.push("u-");
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^") {
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

std::string getInput() {
    std::string infix2;
    std::cout << "Enter an infix expression: ";
    std::getline(std::cin, infix2);
    return infix2;
}


int main() {
    //std::string infix;
    //std::cout << "Enter an infix expression: ";
    //std::getline(std::cin, infix);

    std::string userInput = getInput();
    std::cout << "You entered: " << userInput << std::endl;

    // std::string infix = addSpaces2(userInput);///
    // std::istringstream iss(infix);

    // std::cout << "infix " << infix << std::endl;
    std::istringstream expression_stream(userInput);
    auto rpn = infixToRPN2(expression_stream);




    std::cout << "Reverse Polish Notation: ";
    for (const auto& t : rpn) std::cout << t << ' ';
    std::cout << '\n';

    double result = evaluateRPN2(rpn);

    std::cout << "Result: " << result << '\n';
    return 0;
}
