#include "utils.h"

void print_error(const std::string& str)
{
	std::cerr << str << '\n';
}

void print_except(const std::exception& e)
{
	std::cerr << "Error: " << e.what() << '\n';
}

std::ostream& operator<<(std::ostream& os, const std::list<unsigned int>& list)
{
	for (std::list<unsigned int>::const_iterator it = list.begin();
			it != list.end(); ++it) {
		os << *it;
		if (&(*it) != &(list.back()))
			os << " ";
	}
	return (os);
}

std::ostream& operator<<(std::ostream& os, char** arr)
{
	if (arr) {
		for (int i = 0;	arr[i] != NULL; ) {
			os << arr[i];
			if (arr[++i] != NULL)
				os << " ";
		}
	}
	return (os);
}
