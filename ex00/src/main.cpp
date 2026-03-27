#include "utils.hpp"
#include "BitcoinExchange.hpp"

int main(int argc, char* argv[])
{
	if (argc != 2) {
		print("Usage: btc DB_FILE\n"
				"    DB_FILE: *(DATE '|' VALUE '\\n')\n"
				"    DATE: YEAR-MONTH-DAY => XXXX-XX-XX");
		return (1);
	}

	try {
		BitcoinExchange btc("data.csv");
		btc.values(argv[1]);
	} catch (const std::exception& e) {
		print_except(e);
		return (-1);
	}

	return (0);
}
