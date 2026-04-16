#include "utils.h"

void print_except(const std::exception& e)
{
	std::cerr << "Error: " << e.what() << '\n';
}
