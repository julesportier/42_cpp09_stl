#include "utils.h"
#include "RPN.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		print("Usage: RPN \"REVERSE POLISH NOTATION EXPRESSION\"");
		return (1);
	}
	try {
		RPN expression(argv[1]);
		print(expression.compute());
	} catch (const std::exception& e) {
		print_except(e);
		return (-1);
	}
	return (0);
}
