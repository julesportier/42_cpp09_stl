#include "PmergeMe.h"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <errno.h>

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
PmergeMe::PmergeMe() : m_block_size(1) {};

PmergeMe::PmergeMe(const PmergeMe& src)
	: m_list(src.m_list)
	, m_block_size(src.m_block_size)
{}

PmergeMe::PmergeMe(char** argv)
	: m_argv(argv)
	, m_block_size(1)
{
	if (argv == NULL || argv[0] == NULL)
		throw std::runtime_error("ctor with c array arg cannot be NULL or empty");
	int i = 0;
	for ( ; argv[i] != NULL; ++i) {
		if (i > MAX_INPUT)
			throw std::out_of_range("Input list too big");
	}
	m_input_size = i;
}

PmergeMe::~PmergeMe() {}

PmergeMe& PmergeMe::operator=(const PmergeMe& src)
{
	if (&src != this) {
		m_list = src.m_list;
		m_block_size = src.m_block_size;
	}
	return (*this);
}

/**********
* METHODS *
**********/
// From 0 -> 0, 1, 1, 3...
// Return the difference between Jn and Jn-1.
unsigned int PmergeMe::jacobsthal_diff(unsigned int i) const
{
	if (i == 0)
		return (0);
	if (i > 33) {
		throw std::runtime_error(
			"can't compute Jacobsthal bigger than 33rd into an unsigned int");
	}
	unsigned int j = 1;
	unsigned int j_prev = 0;
	while (--i > 0) {
		unsigned int tmp = j;
		j += j_prev * 2;
		j_prev = tmp;
	}
	return (j - j_prev);
}


PmergeMe::list_t::iterator PmergeMe::to_block_first_node(const list_t::iterator& mid_it)
{
	list_t::iterator it = mid_it;
	if (it == m_list.end()) {
		return (it);
	} else {
		std::advance(it, -(m_block_size - 1));
		return (it);
	}
}

PmergeMe::list_t::iterator PmergeMe::binary_search(
							const list_t::iterator& target_it,
							const list_t::size_type& size)
{
	unsigned int low = 0;
	const size_t main_size = PmergeMe::size(m_list) / m_block_size;
	unsigned int high = (main_size > size ? size - 1 : main_size - 1);
	list_t::iterator mid_it;
	while (low < high) {
		const unsigned int mid = low + (high - low) / 2;
		mid_it = m_list.begin();
		std::advance(mid_it, mid * m_block_size + m_block_size - 1);
		if (*mid_it == *target_it) {
			return (to_block_first_node(mid_it));
		} else if (*mid_it < *target_it) {
			low = (mid < size ? mid + 1 : size - 1);
		} else {
			high = (mid > 0 ? mid - 1 : 0);
		}
	}
	mid_it = m_list.begin();
	std::advance(mid_it, low * m_block_size + m_block_size - 1);
	if (*mid_it > *target_it) {
		std::advance(mid_it, -(m_block_size - 1));
	} else {
		++mid_it;
	}
	return (mid_it);
}

// "pend" must be non empty.
void PmergeMe::binary_insert(list_t pend)
{
	list_t::size_type size = 4;
	unsigned int n_jacob = 3;
	while (PmergeMe::size(pend) > 0) {
		unsigned int p_index = jacobsthal_diff(n_jacob);
		if (PmergeMe::size(pend) / m_block_size < p_index)
			p_index = PmergeMe::size(pend) / m_block_size;
		while (p_index > 0) {
			list_t::iterator pend_it = pend.begin();
			std::advance(pend_it, --p_index * m_block_size);
			block_t block = get_block(pend_it, pend);
			list_t::iterator main_insert_pos = binary_search(block.last, size - 1);
			m_list.splice(main_insert_pos, pend, block.begin, ++block.last);
		}
		size *= 2;
		++n_jacob;
	}
}

// Advance the iterator given as argument past the last element of the block.
PmergeMe::block_t PmergeMe::get_block(
					list_t::iterator& it,
					list_t& list) const
{
	block_t block;

	if (static_cast<unsigned long>(std::distance(it, list.end())) < m_block_size)
		throw std::out_of_range("list too small for block size");
	block.begin = it;
	std::advance(it, m_block_size - 1);
	block.last = it;
	std::advance(it, 1);

	return (block);
}

// Sort pairs between each other.
void PmergeMe::sort_pairs()
{
	list_t::iterator it = m_list.begin();
	while (it != m_list.end() 
			&& static_cast<unsigned long>(std::distance(it, m_list.end()))
				>= m_block_size * 2) {
		// Advance the iterator after the end of the block
		block_t first = get_block(it, m_list);
		block_t second = get_block(it, m_list);

		// Swap blocks.
		if (*first.last > *second.last) {
			m_list.splice(first.begin, m_list, second.begin, it);
		}
	}
}

// Put the little pairs into the pend list to prepare insertion.
// The remaining elements are temporary put into the extra list.
void PmergeMe::split_pairs(list_t& pend, list_t& extra)
{
	list_t::iterator it = m_list.begin();
	while (it != m_list.end() 
			&& static_cast<unsigned long>(std::distance(it, m_list.end()))
				>= m_block_size * 2) {
		block_t first = get_block(it, m_list);
		pend.splice(pend.end(), m_list, first.begin, it);
		if (static_cast<unsigned long>(std::distance(it, m_list.end())) < m_block_size)
			break;
		else
			std::advance(it, m_block_size);
	}
	if (it != m_list.end())
		extra.splice(extra.end(), m_list, it, m_list.end());
}

void PmergeMe::merge_insert_sort_l()
{
	for (list_t::size_type i = 0; m_argv[i] != NULL; ++i) {
		const unsigned int tmp = to_uint(m_argv[i]);
		m_list.push_back(tmp);
	}
	merge_insert_sort_l_impl();
}

void PmergeMe::merge_insert_sort_l_impl()
{
	if (size(m_list) < m_block_size * 2)
		return;

	sort_pairs();

	m_block_size *= 2;
	merge_insert_sort_l_impl();
	m_block_size /= 2;

	list_t pend;
	list_t extra;
	split_pairs(pend, extra);

	// First element on pend always go to the beggining of main (m_list).
	list_t::iterator pend_it = pend.begin();
	block_t first = get_block(pend_it, pend);
	m_list.splice(m_list.begin(), pend, first.begin, pend_it);

	if (size(pend) > 0)
		binary_insert(pend);

	// Insert extra at the end of main.
	if (size(extra) >= m_block_size) {
		list_t::iterator extra_it = extra.begin();
		block_t block = get_block(extra_it, extra);
		list_t::iterator main_insert_pos
			= binary_search(block.last, size(m_list) / m_block_size);
		m_list.splice(main_insert_pos, pend, block.begin, ++block.last);
	}
	if (size(extra) > 0)
		m_list.splice(m_list.end(), extra);
}

unsigned int PmergeMe::to_uint(const char* str) const
{
	if (str == NULL)
		throw std::runtime_error("to_uint() arg cannot be NULL");
	if (str[0] == '-')
		throw std::runtime_error("invalid negative number");
	char* end = NULL;
	errno = 0;
	unsigned long ul = std::strtoul(str, &end, 10);
	if (errno || ul > std::numeric_limits<unsigned int>::max()) {
		errno = 0;
		throw std::runtime_error("out of range number");
	} else if (*end) {
		throw std::runtime_error("unexpected character in number");
	}
	return (static_cast<unsigned int>(ul));
}

/**********
* SETTERS *
**********/
void PmergeMe::list(list_t new_list) { m_list = new_list; }

void PmergeMe::argv(char** argv) { m_argv = argv; }

/**********
* GETTERS *
**********/
const PmergeMe::list_t& PmergeMe::list() const { return (m_list); }

size_t PmergeMe::size(list_t& list)
{
	size_t s = 0;
	for (list_t::iterator it = list.begin(); it != list.end(); ++it)
		++s;
	return (s);
}

size_t PmergeMe::size() { return (m_input_size); }

char** PmergeMe::argv() const { return (m_argv); }
