#include "PmergeMe.h"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <errno.h>

/********
* TYPES *
********/
PmergeMe::List::List() : std::list<unsigned int>() {};

PmergeMe::List::List(const List& src) : std::list<unsigned int>(src) {};

PmergeMe::List& PmergeMe::List::operator=(const List& src)
{
	if (&src != this) {
		*this = src;
	}
	return (*this);
}

PmergeMe::List::~List() {};

PmergeMe::List::iterator PmergeMe::List::at(unsigned int i)
{
	List::iterator it = begin();
	if (i == size()) 
		throw std::out_of_range("can't get end iterator");
	if (i > size()) 
		throw std::out_of_range("can't get iterator out of list size");
	std::advance(it, i);
	return (it);
}

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
PmergeMe::PmergeMe() : m_block_size(1) {};

PmergeMe::PmergeMe(const PmergeMe& src)
	: m_list(src.m_list)
	, m_block_size(src.m_block_size)
{}

PmergeMe::PmergeMe(char** argv)
	: m_block_size(1)
{
	if (argv == NULL)
		throw std::runtime_error("ctor with c array arg cannot be NULL");
	for (list_t::size_type i = 0; argv[i] != NULL; ++i) {
		const unsigned int tmp = to_uint(argv[i]);
		m_list.push_back(tmp);
	}
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

PmergeMe::list_t::iterator PmergeMe::binary_search(
							const list_t::iterator& p_it,
							const list_t::size_type& size)
{
	list_t::iterator m_it = m_list.begin();
	list_t::size_type inter = size / 2 + size % 2;
	std::advance(m_it, inter * m_block_size - 1);

	list_t::const_iterator low = m_list.end();
	list_t::const_iterator high = m_list.end();

	while (1) {
		if (*p_it == *m_it || m_it == high) {
			return (m_it);
		} else if (m_it == low) {
			std::advance(m_it, m_block_size);
			return (m_it);
		} else if (*p_it < *m_it) {
			if (m_it == m_list.begin())
				return (m_it);
			high = m_it;
			inter = inter / 2 + inter % 2;
			std::advance(m_it, -inter * m_block_size);
		} else {
			low = m_it;
			inter = inter / 2 + inter % 2;
			std::advance(m_it, inter * m_block_size);
		}
	}
	return m_it;
}

// "pend" must be non empty.
void PmergeMe::binary_insert(list_t pend)
{
	list_t::size_type size = 4;
	unsigned int n_jacob = 3;
	while (pend.size() > 0) {
		unsigned int p_index = jacobsthal_diff(n_jacob);
		if (pend.size() / m_block_size < p_index)
			p_index = pend.size() / m_block_size;
		while (p_index > 0) {
			list_t::iterator pend_it = pend.begin();
			std::advance(pend_it, --p_index * m_block_size);
			block_t block = get_block(pend_it, pend);
			list_t::iterator main_insert_pos = binary_search(block.last, size - 1);
			if (main_insert_pos != m_list.end())
				std::advance(main_insert_pos, -(m_block_size - 1));
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

void PmergeMe::merge_insert_sort_l()
{
	if (m_list.size() < m_block_size * 2)
		return;

	// Sort pairs between each other.
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
	m_block_size *= 2;
	merge_insert_sort_l();
	m_block_size /= 2;

	// Put the little pairs into the pend list to prepare insertion.
	// The remaining elements are temporary put into the extra list.
	list_t pend;
	list_t extra;
	it = m_list.begin();
	while (it != m_list.end() 
			&& static_cast<unsigned long>(std::distance(it, m_list.end()))
				>= m_block_size) {
		block_t first = get_block(it, m_list);
		pend.splice(pend.end(), m_list, first.begin, it);
		if (static_cast<unsigned long>(std::distance(it, m_list.end())) < m_block_size)
			break;
		else
			std::advance(it, m_block_size);
	}
	if (it != m_list.end())
		extra.splice(extra.end(), m_list, it, m_list.end());

	// First element on pend always go to the beggining of main (m_list).
	list_t::iterator pend_it = pend.begin();
	block_t first = get_block(pend_it, pend);
	m_list.splice(m_list.begin(), pend, first.begin, pend_it);

	if (pend.size() > 0)
		binary_insert(pend);

	// Insert extra at the end of main.
	m_list.splice(m_list.end(), extra);



	/****************************************************************************************************************************
	 * - put one block out of two in a "pend" list
	 * - put remaining nodes (not in a full block or a pair) into a "extra(neous)" list
	 * - put first from "pend" before first pos in "main" as we know it's smaller than it
	 * - starting at i = J4 == 3 put i firsts from "pend" before n - 1 "main" pos (n start = 4)
	 * 		- decrement i until nothing left before
	 * 		- n = n * 2
	 * 		- i = J+1
	 * - if extra size >= m_block_size:
	 *   	binary insert first extra block (if block == 2 and extra size == 3 only insert 2 firsts)
	 * - if remaining in extra:
	 *   	append "extra" at the end of "main"
	 * - Do it at every unrolling of the recursion
	 * 
	 * 
	 * Jacobsthal : 0 1 1 3 5 11 21 43 85 ...
	 * 1  2 .3. 4 .5. 6  7  8  9  10  11
	 * 1b 2b 3b 4b 5b 6b 7b 8b 9b 10b 11b
	 *
	 *    2b 3b 4b 5b 6b 7b 8b 9b 10b 11b -> J 3 (without cmp)
	 * 1b 1 2 .3. 4 .5. 6  7  8  9  10  11
	 *
	 *    	2b    4b 5b 6b 7b 8b 9b 10b 11b -> J 4 == 3 (with cmp). We just compare with the first 3. From 3b.
	 * 1b 3b 1 2 .3. 4 .5. 6  7  8  9  10  11
	 *    	         4b 5b 6b 7b 8b 9b 10b 11b -> 3 - 1 > 1 ok. From 2b in 3 little. Stop after as 1 is preceding Jacob number.
	 * 1b 3b 1 2b 2 .3. 4 .5. 6  7  8  9  10  11
	 *
	 *    	            4b    6b 7b 8b 9b 10b 11b -> J 6 == 5. We just compare with the first 7. From 5b in 7 little.
	 * 1b 3b 1 2b 2 5b .3. 4 .5. 6  7  8  9  10  11
	 *    	                     6b 7b 8b 9b 10b 11b -> 5 - 1 > 3 ok. From 4b in 7 little. Stop after as 3 is preceding Jacob number.
	 * 1b 3b 1 2b 4b 2 5b .3. 4 .5. 6  7  8  9  10  11
	 *
	 *    	                        6b 7b 8b 9b 10b     -> J 7 == 11. We just compare with the first 15. From 11b in 15 little.
	 * 1b 3b 1 2b 4b 2 5b .3. 4 .5. 6 11b 7  8  9  10  11
	 *    	                        6b 7b 8b 9b             -> 11 - 1 == 10 > 7 ok. From 10b in 15 little.
	 * 1b 3b 1 2b 4b 2 5b .3. 4 10b .5. 6 11b 7  8  9  10  11
	 *    	                            6b 7b 8b               -> 10 - 1 == 9 > 7 ok. From 9b in 15 little.
	 * 1b 3b 1 2b 9b 4b 2 5b .3. 4 10b .5. 6 11b 7  8  9  10  11
	 *    	                               6b 7b 8b            -> 9 - 1 == 8 > 7 ok. From 9b in 15 little.
	 *********************************************************************************************************************/
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

/************
* OPERATORS *
************/
std::ostream& operator<<(std::ostream& os, const PmergeMe& rhs)
{
	const PmergeMe::list_t& list = rhs.list();
	for (PmergeMe::list_t::const_iterator it = list.begin();
			it != list.end(); ++it) {
		os << *it;
		if (&(*it) != &(list.back()))
			os << " ";
	}
	return (os);
}

/**********
* SETTERS *
**********/
void PmergeMe::list(list_t new_list) { m_list = new_list; }

/**********
* GETTERS *
**********/
const PmergeMe::list_t& PmergeMe::list() const { return (m_list); }
