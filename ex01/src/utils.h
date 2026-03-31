#ifndef UTILS_H
# define UTILS_H

# include <iostream>

template <typename T>
void print(const T& val)
{
	std::cout << val << '\n';
}

void print_except(const std::exception& e);

// Debug variable printing
template <typename T>
void debug(const char* file,
		int line,
		const char* function,
		const char* var_name,
		const T& var)
{
	std::cout << file << ':' << line << ' ' << function << '\t' 
		<< var_name << " == '" << var << "'\n";
}
# ifdef DBG
#  define DEBUG(var) (debug((__FILE__), (__LINE__), (__FUNCTION__), (#var), (var)))
# else
#  define DEBUG(var)
# endif

#endif
