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
		list.merge_insert_sort_l();
		std::cout << "After: " << list << '\n';

		// PmergeMe sorter;
		// // Start list timer.
		// sorter.list(argv + 1);
		// sorter.merge_insert_sort_l(1);
		// // Stop list timer.
		// // Start deque timer.
		// sorter.deque(argv + 1);
		// sorter.merge_insert_sort_d(1);
		// // Stop deque timer.
	} catch (const std::exception& e) {
		print_except(e);
		return (-1);
	}
	return (0);
}
