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
PmergeMe::list_t PmergeMe::merge(list_t& left, list_t& right)
{
	list_t res;

	list_t::iterator l_it = left.begin();
	list_t::iterator r_it = right.begin();
	while (left.size() > 0 && right.size() > 0) {
		if (left.front() <= right.front())
			res.splice(res.end(), left, l_it++);
		else
			res.splice(res.end(), right, r_it++);
	}

	while (left.size() != 0)
		res.splice(res.end(), left, l_it++);
	while (right.size() != 0)
		res.splice(res.end(), right, r_it++);

	DEBUG(res);
	return (res);
}

PmergeMe::list_t PmergeMe::merge_sort(list_t list)
{
	// DEBUG(list);
	if (list.size() <= 1)
		return (list);

	list_t l;
	list_t r;
	list_t::iterator it = list.begin();
	const list_t::size_type size = list.size();
	for (list_t::size_type i = 0; i < size; ++i) {
		if (i < size / 2)
			l.splice(l.end(), list, it++);
		else
			r.splice(r.end(), list, it++);
	}

	l = merge_sort(l);
	r = merge_sort(r);
	// DEBUG(l);
	// DEBUG(r);

	return (merge(l, r));
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
