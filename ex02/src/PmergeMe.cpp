#include "PmergeMe.h"
#include "utils.h"
#include <limits>
#include <stdexcept>
#include <cstdlib>
#include <errno.h>

/********
* TYPES *
********/

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
PmergeMe::PmergeMe() {}

PmergeMe::PmergeMe(const PmergeMe& src) : m_list(src.m_list) {}

PmergeMe::PmergeMe(char** argv)
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
	}
	return (*this);
}

/**********
* METHODS *
**********/
void PmergeMe::merge_insert_sort_l(list_t::size_type block_size)
{
	// DEBUG(block_size);
	if (m_list.size() < block_size * 2)
		return;

	// Sort pairs between each other.
	list_t::iterator it = m_list.begin();
	while (it != m_list.end() 
			&& static_cast<unsigned long>(std::distance(it, m_list.end()))
				> block_size * 2) {
		// DEBUG(*it);
		// DEBUG(m_list);
		// DEBUG(std::distance(it, m_list.end()));

		// Iterators from the first block.
		const list_t::iterator f = it;
		std::advance(it, block_size - 1);
		const list_t::iterator f_last = it;
		std::advance(it, 1);

		// Iterators from the second block.
		const list_t::iterator s = it;
		std::advance(it, block_size - 1);
		const list_t::iterator s_last = it;
		std::advance(it, 1);

		// Swap blocks.
		if (*f_last > *s_last) {
			DEBUG("swap");
			DEBUG(m_list);
			m_list.splice(f, m_list, s, it);
			DEBUG(m_list);
		}
	}

	merge_insert_sort_l(block_size * 2);

	// Binary insert.
	/****************************************************************************************************************************
	 * - put one block out of two in a "pend" list
	 * - put remaining nodes (not in a full block or a pair) into a "extra(neous)" list
	 * - put first from "pend" before first pos in "main" as we know it's smaller than it
	 * - starting at i = J4 == 3 put i firsts from "pend" before n - 1 "main" pos (n start = 4)
	 * 		- decrement i until nothing left before
	 * 		- n = n * 2
	 * 		- i = J+1
	 * - if extra size >= block_size:
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
