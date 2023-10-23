#include <vector>
#include <stack>
#include <string>
#include <iostream>

bool isOperator(const std::string& s) {
    return s == "+" || s == "-" || s == "*" || s == "/";
}

double applyOperator(double a, double b, const std::string& op) {
    if (op == "+") return a + b;
    if (op == "-") return a - b;
    if (op == "*") return a * b;
    if (op == "/") return a / b;  // Handle division by zero appropriately
    return 0;
}

std::vector<std::string> reduceRPN(const std::vector<std::string>& rpnExpression) {
    std::stack<std::string> st;
    for (const auto& token : rpnExpression) {
        if (isOperator(token)) {
            double operand2 = std::stod(st.top()); st.pop();
            double operand1 = std::stod(st.top()); st.pop();
            double result = applyOperator(operand1, operand2, token);
            st.push(std::to_string(result));
        }
        else {
            st.push(token);
        }
    }

    std::vector<std::string> reducedRPN;
    while (!st.empty()) {
        reducedRPN.insert(reducedRPN.begin(), st.top());
        st.pop();
    }

    return reducedRPN;
}

int main() {
    // Original RPN expression: ["5", "1", "2", "+", "4", "*", "+", "3", "-"]
    std::vector<std::string> rpn = { "5", "1", "2", "+", "4", "*", "+", "3", "-" };

    // Reduce the RPN expression
    std::vector<std::string> reducedRPN = reduceRPN(rpn);

    // Output the reduced RPN expression
    for (const auto& token : reducedRPN) {
        std::cout << token << ' ';
    }
    std::cout << std::endl;

    return 0;
}
