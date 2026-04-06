#ifndef PMERGEME_H
# define PMERGEME_H

# include <list>
# include <ostream>

class PmergeMe {
public:
	typedef std::list<unsigned int> list_t;
	struct block_t {
		list_t::iterator begin;
		list_t::iterator last;
	};

	PmergeMe();
	PmergeMe(char** argv);
	PmergeMe(const PmergeMe& src);
	~PmergeMe();
	PmergeMe& operator=(const PmergeMe& src);

	void merge_insert_sort_l(list_t::size_type block_size);

	const list_t& list() const;
	void list(list_t new_list);
private:
	list_t m_list;
	unsigned int to_uint(const char* str) const;
	block_t get_block(
				list_t::iterator& it,
				list_t::size_type block_size) const;
};

std::ostream& operator<<(std::ostream& os, const PmergeMe& rhs);

#endif
