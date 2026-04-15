#ifndef PMERGEME_H
# define PMERGEME_H

# define MAX_INPUT 3000

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

	void merge_insert_sort_l();
	void merge_insert_sort_l_impl();

	const list_t& list() const;
	static size_t size(list_t& list);
	size_t size();
	char** argv() const;
	void argv(char** argv);
private:
	char **m_argv;
	unsigned int m_input_size;
	list_t m_list;
	list_t::size_type m_block_size;
	unsigned int to_uint(const char* str) const;
	void sort_pairs();
	void split_pairs(list_t& pend, list_t& extra);
	block_t get_block(list_t::iterator& it, list_t& list) const;
	list_t::iterator to_block_first_node(const list_t::iterator& mid_it);
	unsigned int jacobsthal_diff(unsigned int i) const;
	PmergeMe::list_t::iterator binary_search(
								const list_t::iterator& p_it,
								const list_t::size_type& size);
	void binary_insert(list_t pend);
};

#endif
