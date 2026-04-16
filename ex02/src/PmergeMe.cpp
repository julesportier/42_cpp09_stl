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
	int i = 1;
	for ( ; argv[i] != NULL; ++i) {
		if (i >= MAX_INPUT)
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
			block_l_t block = get_block(pend_it, pend);
			list_t::iterator main_insert_pos = binary_search(block.last, size - 1);
			m_list.splice(main_insert_pos, pend, block.begin, ++block.last);
		}
		size *= 2;
		++n_jacob;
	}
}

// Advance the iterator given as argument past the last element of the block.
PmergeMe::block_l_t PmergeMe::get_block(
					list_t::iterator& it,
					list_t& list) const
{
	block_l_t block;

	if (static_cast<unsigned long>(std::distance(it, list.end())) < m_block_size)
		throw std::out_of_range("list too small for block size");
	block.begin = it;
	std::advance(it, m_block_size - 1);
	block.last = it;
	std::advance(it, 1);

	return (block);
}

// Sort pairs between each other.
void PmergeMe::sort_pairs_l()
{
	list_t::iterator it = m_list.begin();
	while (it != m_list.end() 
			&& static_cast<unsigned long>(std::distance(it, m_list.end()))
				>= m_block_size * 2) {
		// Advance the iterator after the end of the block
		block_l_t first = get_block(it, m_list);
		block_l_t second = get_block(it, m_list);

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
		block_l_t first = get_block(it, m_list);
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

	sort_pairs_l();

	m_block_size *= 2;
	merge_insert_sort_l_impl();
	m_block_size /= 2;

	list_t pend;
	list_t extra;
	split_pairs(pend, extra);

	// First element on pend always go to the beggining of main (m_list).
	list_t::iterator pend_it = pend.begin();
	block_l_t first = get_block(pend_it, pend);
	m_list.splice(m_list.begin(), pend, first.begin, pend_it);

	if (size(pend) > 0)
		binary_insert(pend);

	// Insert extra at the end of main.
	if (size(extra) >= m_block_size) {
		list_t::iterator extra_it = extra.begin();
		block_l_t block = get_block(extra_it, extra);
		list_t::iterator main_insert_pos
			= binary_search(block.last, size(m_list) / m_block_size);
		m_list.splice(main_insert_pos, pend, block.begin, ++block.last);
	}
	if (size(extra) > 0)
		m_list.splice(m_list.end(), extra);
}

void PmergeMe::merge_insert_sort_d()
{
	for (deque_t::size_type i = 0; m_argv[i] != NULL; ++i) {
		const unsigned int tmp = to_uint(m_argv[i]);
		m_deque.push_back(tmp);
	}
	merge_insert_sort_d_impl();
}

void PmergeMe::merge_insert_sort_d_impl()
{
	if (m_deque.size() < m_block_size * 2)
		return;

	sort_pairs_d();

	m_block_size *= 2;
	merge_insert_sort_d_impl();
	m_block_size /= 2;


	deque_t pend;
	deque_t extra;
	split_pairs(pend, extra);

	// First element on pend always go to the beggining of main (m_list).
	move_block(m_deque, pend, 0, 0);

	if (pend.size() > 0)
		binary_insert(pend);

	// Insert extra at the end of main.
	if (extra.size() >= m_block_size) {
		deque_t::size_type main_insert_pos = binary_search(extra, 0, m_deque.size() / m_block_size);
		move_block(m_deque, extra, 0, main_insert_pos);
	}
	if (extra.size() > 0)
		move_to_end(m_deque, extra, 0, extra.size());
}

PmergeMe::deque_t::size_type PmergeMe::get_block_big(deque_t::size_type block)
{
	return (block * m_block_size + m_block_size - 1);
}

PmergeMe::deque_t::size_type PmergeMe::binary_search(
								const deque_t& source,
								const deque_t::size_type& source_index,
								const deque_t::size_type& size)
{
	unsigned int low = 0;
	const deque_t::size_type main_size = m_deque.size() / m_block_size;
	unsigned int high = (main_size > size ? size - 1 : main_size - 1);
	unsigned int insert_val = source.at(get_block_big(source_index));
	while (low < high) {
		const unsigned int mid = low + (high - low) / 2;
		const unsigned int mid_block = get_block_big(mid);
		if (m_deque.at(mid_block) == insert_val) {
			return (mid_block - (m_block_size - 1));
		} else if (m_deque.at(mid_block) < insert_val) {
			low = (mid < size ? mid + 1 : size - 1);
		} else {
			high = (mid > 0 ? mid - 1 : 0);
		}
	}
	deque_t::size_type insert_pos = get_block_big(low);
	if (m_deque.at(insert_pos) > insert_val) {
		insert_pos -= m_block_size - 1;
	} else {
		++insert_pos;
	}
	return (insert_pos);
}

// "pend" must be non empty.
void PmergeMe::binary_insert(deque_t& pend)
{
	list_t::size_type size = 4;
	unsigned int n_jacob = 3;
	while (pend.size() > 0) {
		unsigned int p_index = jacobsthal_diff(n_jacob);
		if (pend.size() / m_block_size < p_index)
			p_index = pend.size() / m_block_size;
		while (p_index > 0) {
			--p_index;
			list_t::size_type main_insert_pos = binary_search(pend, p_index, size - 1);
			move_block(m_deque, pend, p_index, main_insert_pos);
		}
		size *= 2;
		++n_jacob;
	}
}

void PmergeMe::move(
		deque_t& target,
		deque_t& source,
		const deque_t::size_type& s_start,
		const deque_t::size_type& s_end,
		const deque_t::size_type& insert_pos)
{
	deque_t::iterator start = source.begin();
	std::advance(start, s_start);
	deque_t::iterator end = source.begin();
	std::advance(end, s_end);
	deque_t::iterator ins = target.begin();
	std::advance(ins, insert_pos);
	target.insert(ins, start, end);
	source.erase(start, end);
}

void PmergeMe::move_block(
		deque_t& target,
		deque_t& source,
		const deque_t::size_type& block_nbr,
		const deque_t::size_type& insert_pos)
{
	move(target,
		source,
		block_nbr * m_block_size,
		(block_nbr + 1) * m_block_size,
		insert_pos);
}

void PmergeMe::move_to_end(
		deque_t& target,
		deque_t& source,
		const deque_t::size_type& s_start,
		const deque_t::size_type& s_end)
{
	move(target, source, s_start, s_end, target.size());
}

void PmergeMe::move_block_to_end(
		deque_t& target,
		deque_t& source,
		const deque_t::size_type& block_nbr)
{
	move_to_end(target,
				source,
				block_nbr * m_block_size,
				(block_nbr + 1) * m_block_size);
}

// Put the little pairs into the pend deque to prepare insertion.
// The remaining elements are temporary put into the extra deque.
void PmergeMe::split_pairs(deque_t& pend, deque_t& extra)
{
	deque_t::size_type i = 0;
	for ( ; (i + 2) * m_block_size <= m_deque.size(); ++i) {
		move_block_to_end(pend, m_deque, i);
	}
	if (m_deque.size() > i * m_block_size) {
		move_to_end(extra, m_deque, i * m_block_size, m_deque.size());
	}
}

void PmergeMe::swap_blocks(const deque_t::size_type& i)
{
	// Create sublist to insert.
	deque_t::iterator begin = m_deque.begin();
	std::advance(begin, i * m_block_size + m_block_size);
	deque_t::iterator end = begin;
	std::advance(end, m_block_size);
	deque_t tmp;
	tmp.assign(begin, end);
	// Delete sublist from m_deque.
	m_deque.erase(begin, end);
	// Insert tmp sublist to correct pos in m_deque.
	deque_t::iterator ins = m_deque.begin();
	std::advance(ins, i * m_block_size);
	m_deque.insert(ins, tmp.begin(), tmp.end());
}

// Advance the iterator given as argument past the last element of the block.
PmergeMe::block_d_t PmergeMe::get_block(
					deque_t::iterator& it,
					deque_t& deque) const
{
	block_d_t block;

	if (static_cast<unsigned long>(std::distance(it, deque.end())) < m_block_size)
		throw std::out_of_range("deque too small for block size");
	block.begin = it;
	std::advance(it, m_block_size - 1);
	block.last = it;
	std::advance(it, 1);

	return (block);
}

// Sort pairs between each other.
void PmergeMe::sort_pairs_d()
{
	for (deque_t::size_type i = 0;
			(i + 2) * m_block_size <= m_deque.size();
			i += 2) {
		if (m_deque.at(get_block_big(i)) 
				> m_deque.at(get_block_big(i + 1))) {
			swap_blocks(i);
		}
	}
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
void PmergeMe::argv(char** argv) { m_argv = argv; }

/**********
* GETTERS *
**********/
const PmergeMe::list_t& PmergeMe::list() const { return (m_list); }

const PmergeMe::deque_t& PmergeMe::deque() const { return (m_deque); }

size_t PmergeMe::size(list_t& list)
{
	size_t s = 0;
	for (list_t::iterator it = list.begin(); it != list.end(); ++it)
		++s;
	return (s);
}

size_t PmergeMe::size() { return (m_input_size); }

char** PmergeMe::argv() const { return (m_argv); }
