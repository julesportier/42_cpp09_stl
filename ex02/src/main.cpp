#include "utils.h"
#include "PmergeMe.h"

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print("Usage: PmergeMe LIST_TO_SORT");
		return (1);
	}
	try {
		PmergeMe list(argv + 1);
		std::cout << "Before: " << list << '\n';
		PmergeMe::list_t l = list.list();
		list.list(list.merge_sort(l));
		std::cout << "After: " << list << '\n';
	} catch (const std::exception& e) {
		print_except(e);
		return (-1);
	}
	return (0);
}
