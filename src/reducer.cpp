
std::string generate_random_expr(int depth) {
    if (depth > 0 && ((double)rand() / (RAND_MAX)) < 0.5) {
        // 50% chance to choose unary or binary operator
        if ((double)rand() / (RAND_MAX) < 0.5) {
            std::string op = binary_operators[random_int(0, binary_operators.size() - 1)];
            return "(" + generate_random_expr(depth - 1) + " " + op + " " + generate_random_expr(depth - 1) + ")";
        }
        else {
            std::string op = unary_operators[random_int(0, unary_operators.size() - 1)];
            return op + "(" + generate_random_expr(depth - 1) + ")";
        }
    }
    else {
        /// 50% chance to choose a constant or a terminal
        if ((double)rand() / (RAND_MAX) < 0.5) {
            return "(" + constants[random_int(0, constants.size() - 1)] + ")";
        }
        else {
            return "(" + terminals[random_int(0, terminals.size() - 1)] + ")";
        }
    }
}


int main() {
	std::vector<std::string> expressions = create_initial_population(population_size, depth);

	return 0;
}