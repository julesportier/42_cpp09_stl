#ifndef UTILS_HPP
# define UTILS_HPP

# include <iostream>

void print(const std::string& str);
void print_except(const std::exception& e);

std::string& trim(std::string& str, const std::string sep = " \t");

unsigned int to_uint(const std::string& str);
double to_pos_double(const std::string& str);

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
