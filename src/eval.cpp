#include <string>
#include <vector>  
#include <algorithm>

#include <iostream>
#include <string>
#include <stack>

std::string reverse(const std::string& input) {
    std::string reversed;
    for (std::size_t i = input.length(); i > 0; i--) {
        reversed += input[i - 1];
    }
    return reversed;
}

std::string expand_parenthesis(const std::string& expr) {
    std::stack<std::string> operands;
    std::stack<char> operators;
    std::string output = "";
    std::string temp = "";
    std::vector<char> binary_operators = { '+', '-', '*', '/', '^' };

    for (int i = 0; i < expr.length(); i++) {
        char ch = expr[i];
        if (ch == '(' && i > 0 && expr[i - 1] == '*') {
            for (int j = i; j < expr.length(); j++) {
                if (expr[j] == ')') {
                    break;
                }
                else {
                    if (std::find(binary_operators.begin(), binary_operators.end(), expr[j]) != binary_operators.end()) {
                        operators.push(expr[j]);
                    }
                    else {
                        operands.push(std::string(1, expr[j]));
                    }
                }
            }
            for (int k = i-1; k >= 0; k--)
            {
                if (std::find(binary_operators.begin(), binary_operators.end(), expr[k]) != binary_operators.end() && expr[k] != '-' && expr[k] != '*') {
					//temp += expr[k];
					break;
				}
                else {
					temp += expr[k];
                    std::cout << expr[k] << std::endl;
				}
                
			}
        }
    }
    
    //Evaluate each the stack
    //get the inverse of a string temp

    // std::cout << "temp: " << temp << std::endl;

    std::string inverse_temp = reverse(temp);
    // std::cout << "inverse_temp: " << inverse_temp << std::endl;

    int len_operands = operands.size();
    int len_operators = operators.size();

    for (int i = 0; i < len_operands; i++) {
		std::cout << "Ope: " << operands.top() << std::endl;
		operands.pop();
	}

    // print the inverse of temp
    



    return temp;
}

int main() {
    std::string expr = "-2*X*(X - Y)"; // "-2*X*(X - Y) - 2*X - Y - 69"
    std::string expanded = expand_parenthesis(expr);
    std::cout << "Expanded expression: " << expanded << std::endl;
    return 0;
}
