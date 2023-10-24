#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <sstream>
#include <cmath>
#include <vector>


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

struct Node {
    std::string data;
    Node* left;
    Node* right;
};

bool isOperator(const std::string& c) {
    return (c == "+" || c == "-" || c == "*" || c == "/");
}

Node* newNode(const std::string& v) {
    Node* temp = new Node;
    temp->left = temp->right = nullptr;
    temp->data = v;
    return temp;
}

bool isConstant(const std::string& str) {
    // Checks if the string represents a constant (number)
    // Here, we're just checking for digits; you might want to extend this
    for (char c : str) {
        if (!std::isdigit(c) && c != '.') {
            return false;
        }
    }
    return true;
}

Node* reduceExpression(Node* root) {
    if (!root) return nullptr;

    // Reduce left and right subtrees first
    root->left = reduceExpression(root->left);
    root->right = reduceExpression(root->right);

    // If both children are constants, evaluate the expression rooted at this node
    if (root->left && root->right && isConstant(root->left->data) && isConstant(root->right->data)) {
        double left = std::stod(root->left->data);
        double right = std::stod(root->right->data);
        double result = 0.0;

        if (root->data == "+") {
            result = left + right;
        }
        else if (root->data == "-") {
            result = left - right;
        }
        else if (root->data == "*") {
            result = left * right;
        }
        else if (root->data == "/" && right != 0.0) {
            result = left / right;
        }

        // Replace this subtree with a single node containing the result
        delete root->left;
        delete root->right;
        root->left = root->right = nullptr;
        root->data = std::to_string(result);

        // Remove trailing decimal zeros (and decimal point if it's an integer)
        root->data.erase(root->data.find_last_not_of('0') + 1, std::string::npos);
        if (root->data.back() == '.') {
            root->data.pop_back();
        }
    }

    return root;
}

void inorder(Node* t) {
    if (t) {
        inorder(t->left);
        std::cout << t->data << ' ';
        inorder(t->right);
    }
}


std::string infixToRPN_string(std::istringstream& infix) {
    std::stack<std::string> operators;
    std::string rpn;
    std::string token;
    std::string lastToken = "("; // Initialize to left parenthesis for unary negation checks

    while (infix >> token) {
        if (token == "-" && (lastToken == "(" || precedence2(lastToken) > 0)) {
            operators.push("u-");
        }
        else if (token == "+" || token == "-" || token == "*" || token == "/" || token == "^" || token == "sin" || token == "cos" || token == "exp") {
            while (!operators.empty() && precedence2(operators.top()) >= precedence2(token)) {
                rpn += operators.top() + " ";
                operators.pop();
            }
            operators.push(token);
        }
        else if (token == "(") {
            operators.push(token);
        }
        else if (token == ")") {
            while (!operators.empty() && operators.top() != "(") {
                rpn += operators.top() + " ";
                operators.pop();
            }
            operators.pop();
        }
        else {
            rpn += token + " ";
        }
        lastToken = token;
    }

    while (!operators.empty()) {
        rpn += operators.top() + " ";
        operators.pop();
    }

    if (!rpn.empty()) {
        rpn.pop_back();  // Remove the trailing space
    }

    return rpn;
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

int main() {
    // Assume this postfix expression is derived from your original expression
    // "1/2+(X)/(sin(9-2)^2-6/7)"

    std::string infix = addSpaces("7 + Y + Y * Y + 8.5 - Y + 8.5 + Y + 8 - Y - X");
    std::istringstream iss2(infix); 

    std::string rpn = infixToRPN_string(iss2);
    std::cout << "RPN: " << rpn << std::endl;
    
    



    std::cout << std::endl;

    // std::string postfixExpression = "2 X * 2 1 + + 2 9 * +"; // This is just an example
    // std::istringstream iss(postfixExpression);
    
    std::istringstream iss(rpn);
    std::stack<Node*> st;
    std::string token;

    while (iss >> token) {
        if (isOperator(token)) {
            Node* t = newNode(token);
            t->right = st.top(); st.pop();
            t->left = st.top(); st.pop();
            st.push(t);
        }
        else {
            st.push(newNode(token));
        }
    }

    Node* root = st.top();
    std::cout << "Original expression: ";
    inorder(root);
    std::cout << std::endl;

    root = reduceExpression(root);
    std::cout << "Reduced expression: ";
    inorder(root);
    std::cout << std::endl;

    return 0;
}
