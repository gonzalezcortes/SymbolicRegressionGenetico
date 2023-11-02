#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include "external/omp.h"

// Dummy function to simulate some computation
void myFunction(int id, int N) {
    //std::cout << "Evaluating function " << id << std::endl;
    long long sum = 0;
    for (int i = 1; i <= N; ++i) {
        sum += i;
    }
    //std::cout << "Result of function " << id << ": " << sum << std::endl;
}

std::vector<double> generate_synthetic_data(int n) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<> dist(0.0, 1.0);

    std::vector<double> synthetic_data(n);
    for (int i = 0; i < n; i++) {
        synthetic_data[i] = dist(gen);
    }

    return synthetic_data;
}

int main() {
    // Create a synthetic dataset with random data n = 100
    std::vector<double> synthetic_data = generate_synthetic_data(100000000);

    auto start_time = std::chrono::high_resolution_clock::now();  // Start timing

    // Use OpenMP to parallelize the loop
    #pragma omp parallel for
    for (int i = 0; i < synthetic_data.size(); ++i) {
        myFunction(i, synthetic_data[i]);
    }

    auto end_time = std::chrono::high_resolution_clock::now();  // End timing

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
    std::cout << "Total time taken: " << duration.count() << " microseconds" << std::endl;

    auto start_time1 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < synthetic_data.size(); ++i) {
        myFunction(i, synthetic_data[i]);
    }

    auto end_time1 = std::chrono::high_resolution_clock::now();  // End timing

    auto duration1 = std::chrono::duration_cast<std::chrono::microseconds>(end_time1 - start_time1);

    std::cout << "Total time taken: " << duration1.count() << " microseconds" << std::endl;

    return 0;
}
