#include <iostream>
#include <vector>
#include <algorithm>
#include <execution>

// Define a simple struct to hold your data
struct Data {
    double Z;
    double Y;
};

int main() {
    // Example dataset
    std::vector<Data> df = {
        {1.0, 3.0},
        {2.0, 4.0},
        {5.0, 1.0}
        // Add more data as needed
    };

    // Reserve space for results to avoid reallocations
    std::vector<double> results;
    results.reserve(df.size());

    // Compute Z + Y + 2 for each row in parallel
    std::transform(std::execution::par, df.begin(), df.end(), std::back_inserter(results), [](const Data& row) {
        return row.Z + row.Y + 2;
        });

    // Output the results
    std::cout << "Results:" << std::endl;
    for (const auto& result : results) {
        std::cout << result << std::endl;
    }

    return 0;
}
