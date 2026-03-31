#include "utils.h"

void print_except(const std::exception& e)
{
	std::cout << "Error: " << e.what() << '\n';
}
