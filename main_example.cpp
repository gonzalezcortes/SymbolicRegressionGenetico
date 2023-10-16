#include <iostream>
#include <string>
#include <vector>
#include <cstdint>




//function that sum two numbers


std::int16_t sum(std::int16_t a, std::int16_t b) {
	return a + b;
}


// funtion to input sum
int main()
{
	int a, b, c;
	std::cout << "Enter two numbers to add\n";
	std::cin >> a >> b;
	c = sum(a,b);
	std::cout << "Sum of the numbers: " << c << std::endl;
	return 0;
}