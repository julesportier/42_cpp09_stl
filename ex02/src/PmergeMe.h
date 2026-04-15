#ifndef PMERGEME_H
# define PMERGEME_H

# define MAX_INPUT 3000

# include <deque>
# include <list>

class PmergeMe {
public:
	typedef std::list<unsigned int> list_t;
	typedef std::deque<unsigned int> deque_t;
	struct block_l_t {
		list_t::iterator begin;
		list_t::iterator last;
	};
	struct block_d_t {
		deque_t::iterator begin;
		deque_t::iterator last;
	};

	PmergeMe();
	PmergeMe(char** argv);
	PmergeMe(const PmergeMe& src);
	~PmergeMe();
	PmergeMe& operator=(const PmergeMe& src);

	void merge_insert_sort_l();
	void merge_insert_sort_l_impl();

	void merge_insert_sort_d();
	void merge_insert_sort_d_impl();

	const list_t& list() const;
	const deque_t& deque() const;
	static std::size_t size(list_t& list);
	std::size_t size();
	char** argv() const;
	void argv(char** argv);
private:
	char **m_argv;
	unsigned int m_input_size;
	list_t m_list;
	deque_t m_deque;
	list_t::size_type m_block_size;
	unsigned int to_uint(const char* str) const;
	void sort_pairs_l();
	void split_pairs(list_t& pend, list_t& extra);
	block_l_t get_block(list_t::iterator& it, list_t& list) const;
	block_d_t get_block(deque_t::iterator& it, deque_t& deque) const;
	list_t::iterator to_block_first_node(const list_t::iterator& mid_it);
	unsigned int jacobsthal_diff(unsigned int i) const;
	PmergeMe::list_t::iterator binary_search(
								const list_t::iterator& p_it,
								const list_t::size_type& size);
	void binary_insert(list_t pend);
	void sort_pairs_d();
	void split_pairs(deque_t& pend, deque_t& extra);
	void binary_insert(deque_t& pend);
	PmergeMe::deque_t::size_type binary_search(
									const deque_t& source,
									const deque_t::size_type& source_index,
									const deque_t::size_type& size);
	PmergeMe::deque_t::size_type get_block_big(deque_t::size_type block);
	void swap_blocks(const deque_t::size_type& i);
	void move(
			deque_t& target,
			deque_t& source,
			const deque_t::size_type& s_start,
			const deque_t::size_type& s_end,
			const deque_t::size_type& insert_pos);
	void move_block(
			deque_t& target,
			deque_t& source,
			const deque_t::size_type& block_nbr,
			const deque_t::size_type& insert_pos);
	void move_to_end(
			deque_t& target,
			deque_t& source,
			const deque_t::size_type& s_start,
			const deque_t::size_type& s_end);
	void move_block_to_end(
			deque_t& target,
			deque_t& source,
			const deque_t::size_type& block_nbr);
};

#endif
