
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

std::vector<std::string> infixToRPN3(std::istringstream& infix) {
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






int main(){

    std::string replaced_str = "- 2 + 2"; // string with the replaced expression
    std::string infix = addSpaces(replaced_str); // add spaces to the expression
    std::istringstream iss(infix); // create a string stream with the expression
    std::vector<std::string> rpn = infixToRPN3(iss);
    // print rpn
    for (auto i : rpn) {
		std::cout << i << " ";
	}
    std::cout << std::endl;



    std::vector<std::string> rpn2 = { "0", "2", "sin","-" "2", "+" };
    std::cout << evaluateRPN2(rpn2) << std::endl;

    return 0;


}