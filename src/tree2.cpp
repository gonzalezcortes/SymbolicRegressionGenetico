#include <iostream>
#include <string>
#include <stack>
#include <cctype>
#include <sstream>
#include <cmath>

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
    for (char c : str) {
        if (!std::isdigit(c) && c != '.') {
            return false;
        }
    }
    return true;
}

Node* reduceExpression(Node* root) {
    // ... (Same as your original code)
    // ...
    return root;
}

std::string inorder(Node* t) {
    if (!t) return "";
    std::string left = inorder(t->left);
    std::string right = inorder(t->right);
    if (left.empty() && right.empty()) {
        return t->data;
    }
    return "(" + left + " " + t->data + " " + right + ")";
}

std::string reduceExpressionString(const std::string& postfixExpression) {
    std::istringstream iss(postfixExpression);
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
    root = reduceExpression(root);
    return inorder(root);
}



std::string postfixToInfix(const std::string& postfixExpression) {
    std::istringstream iss(postfixExpression);
    std::stack<std::string> st;
    std::string token;

    while (iss >> token) {
        if (isOperator(token)) {
            std::string right = st.top(); st.pop();
            std::string left = st.top(); st.pop();
            std::string infix = "(" + left + " " + token + " " + right + ")";
            st.push(infix);
        }
        else {
            st.push(token);
        }
    }

    return st.top();
}

int main() {
    std::string postfixExpression = "3 7 + 8 X * + 2 + 3 +"; // Example
    std::cout << "Original expression: " << postfixExpression << std::endl;

    std::string reduced = reduceExpressionString(postfixExpression);
    std::cout << "Reduced expression: " << reduced << std::endl;

    std::string infix = postfixToInfix(postfixExpression);
    std::cout << "Infix expression: " << infix << std::endl;

    return 0;
}
