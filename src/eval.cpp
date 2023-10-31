#include <iostream>
#include <cstdlib>
#include <ctime>

#include <chrono>

void print_mutation() {
    std::cout << "mutation" << std::endl;
}
void print_prune_left() {
	std::cout << "prune left" << std::endl;
}
void print_prune_right() {
	std::cout << "prune right" << std::endl;
}
void print_grow_left() {
	std::cout << "grow left" << std::endl;
}
void print_grow_right() {
	std::cout << "grow right" << std::endl;
}
void print_no_mutation() {
	std::cout << "no mutation" << std::endl;
}


double get_random() {
    return static_cast<double>(rand()) / RAND_MAX;
}

void handle_operation(double random_val, double probability, void (*action_true)(), void (*action_false)()) {
    if (random_val < probability) {
        action_true();
    }
    else {
        action_false();
    }
}

void handle_binary_operation(double random_val, void (*action_left)(), void (*action_right)()) {
    if (random_val < 0.5) {
        action_left();
    }
    else {
        action_right();
    }
}

void crossover(double mutation_prob, double prune_prob, double grow_prob, double crossover_prob) {

    for (int i = 0; i < 8; i++) {

        double rand_mutation = ((double)rand() / (RAND_MAX));
        double rand_prune = ((double)rand() / (RAND_MAX));

        double rand_grow = ((double)rand() / (RAND_MAX));
        double rand_crossover = ((double)rand() / (RAND_MAX));


        if (rand_mutation < mutation_prob) {
            print_mutation();
        }
        else {
            print_no_mutation();
        }
        if (rand_prune < prune_prob) {
            if (((double)rand() / (RAND_MAX)) < 0.5) {
                print_prune_left();
            }
            else {
                print_prune_right();
            }
        }
        else {
            std::cout << "no prune" << std::endl;
        }

        if (rand_grow < grow_prob) {
            if (((double)rand() / (RAND_MAX)) < 0.5) {
                print_grow_left();
            }
            else {
                print_grow_right();
            }
        }
        else { std::cout << "no grow" << std::endl;; }

        if (rand_crossover < crossover_prob) {
            std::cout << "crossover" << std::endl;
        }
        else { std::cout << "no crossover" << std::endl; }
    }
}


void crossover2(double mutation_prob, double prune_prob, double grow_prob, double crossover_prob) {

    for(int i = 0; i < 8; i++){

        handle_operation(get_random(), mutation_prob, print_mutation, print_no_mutation);

        if (get_random() < prune_prob) {
            handle_binary_operation(get_random(), print_prune_left, print_prune_right);
        }
        else {
            std::cout << "no prune" << std::endl;
            return; 
        }

        handle_operation(get_random(), grow_prob,
            []() { handle_binary_operation(get_random(), print_grow_left, print_grow_right); },
            []() { std::cout << "no grow" << std::endl; }
        );

        handle_operation(get_random(), crossover_prob,
            []() { std::cout << "crossover" << std::endl; },
            []() { std::cout << "no crossover" << std::endl; }
        );
    }
}

int main() {
    srand(time(0));  // Seed the random number generator

    double mutation_prob = 0.1;
    double prune_prob = 0.1;
    double grow_prob = 0.1;
    double crossover_prob = 0.1;


    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 100; i++) {
        std::cout << "iteration " << i << std::endl;
        crossover2(mutation_prob, prune_prob, grow_prob, crossover_prob);
        std::cout << " " << std::endl;
    }

    auto stop = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);


    std::cout << duration.count() << std::endl;
    return 0;
}