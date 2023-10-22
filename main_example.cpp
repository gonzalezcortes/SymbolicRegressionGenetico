#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

class Training {
private:
    double X_array[5][3] = { {2,20,200}, {3,30,300}, {4,40,400}, {5,50,500}, {6,60,600} };

    std::vector<double> y_vector;
    int rows_x = sizeof(X_array) / sizeof(X_array[0]);
    int columns_x = sizeof(X_array[0]) / sizeof(X_array[0][0]);

    std::vector<std::string> terminals;

public:
    Training() {
        // Initialize terminals within the class constructor
        terminals = { "X", "Y", "Z" };
    }

    std::vector<double> evaluationArray(std::vector<std::string> rpn) {
        std::cout << "rpn: ";
        for (const auto& token : rpn) {
            std::cout << token << ' ';
        }
        std::cout << std::endl;

        std::vector<double> evaluation_vector;
        evaluation_vector.reserve(rows_x);

        int n_terminals = terminals.size();

        for (int i = 0; i < rows_x; i++) {
            std::vector<std::string> rpn_copy = rpn;  // Create a fresh copy each time
            for (std::string& token : rpn_copy) {
                auto it = std::find(terminals.begin(), terminals.end(), token);
                if (it != terminals.end()) {
                    int position = it - terminals.begin();
                    double value = X_array[i][position];
                    
                    std::cout << token << " changed by " << value << '\n';
                    
                    token = std::to_string(value);
                     
                }
            }
            std::cout << "new rpn (rpn_copy): ";
            for (const auto& token : rpn_copy) {
                std::cout << token << ' ';
            }
            std::cout << std::endl;
            
        }
        return evaluation_vector;
    }
};

int main() {
    std::vector<std::string> rpn = { "7", "X", "X", " + " };
    Training training;

    std::vector<double> evaluation_vector = training.evaluationArray(rpn);

    for (const auto& val : evaluation_vector) {
        std::cout << val << ' ';
    }

    return 0;
}
