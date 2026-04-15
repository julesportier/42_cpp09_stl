#include "utils.h"
#include "PmergeMe.h"
#include <ctime>

int main(int argc, char *argv[])
{
	if (argc < 2) {
		print_error("Usage: PmergeMe LIST_TO_SORT");
		return (1);
	}
	try {
		PmergeMe merge_insert(argv + 1);
		std::cout << "Before: " << merge_insert.argv() << '\n';

		const std::clock_t list_start = std::clock();
		merge_insert.merge_insert_sort_l();
		const std::clock_t list_end = std::clock();
		const std::clock_t list_time = list_end - list_start;
		std::cout << "After:  " << merge_insert.list() << '\n';
		std::cout << "Time to process a range of " << merge_insert.size()
			<< " elements with std::list : " << list_time << " us\n";

		const std::clock_t deque_start = std::clock();
		merge_insert.merge_insert_sort_d();
		const std::clock_t deque_end = std::clock();
		const std::clock_t deque_time = deque_end - deque_start;
		// std::cout << "After:  " << merge_insert.deque() << '\n';
		std::cout << "Time to process a range of " << merge_insert.size()
			<< " elements with std::deque : " << deque_time << " us\n";
	} catch (const std::exception& e) {
		print_except(e);
		return (-1);
	}
	return (0);
}
