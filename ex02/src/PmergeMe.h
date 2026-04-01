#ifndef PMERGEME_H
# define PMERGEME_H

# include <list>
# include <ostream>

class PmergeMe {
public:
	typedef std::list<unsigned int> list_t;

	PmergeMe();
	PmergeMe(char** argv);
	PmergeMe(const PmergeMe& src);
	~PmergeMe();
	PmergeMe& operator=(const PmergeMe& src);

	list_t merge(list_t& left, list_t& right);
	list_t merge_sort(list_t list);

	const list_t& list() const;
	void list(list_t new_list);
private:
	list_t m_list;
	unsigned int to_uint(const char* str) const;
};

std::ostream& operator<<(std::ostream& os, const PmergeMe& rhs);

#endif
