#include "utils.hpp"

void print(const std::string& str)
{
	std::cout << str << '\n';
}

void print_except(const std::exception& e)
{
	std::cout << "Error: " << e.what() << '\n';
}
