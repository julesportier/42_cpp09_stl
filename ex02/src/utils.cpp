#include "utils.h"

void print_except(const std::exception& e)
{
	std::cout << "Error: " << e.what() << '\n';
}

std::ostream& operator<<(std::ostream& os, const std::list<unsigned int>& list)
{
	os << '[';
	for (std::list<unsigned int>::const_iterator it = list.begin();
			it != list.end(); ++it) {
		os << *it;
		if (&(*it) != &(list.back()))
			os << " ";
	}
	os << ']';
	return (os);
}
