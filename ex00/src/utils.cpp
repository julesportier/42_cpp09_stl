#include "utils.hpp"
#include <cstdlib>
#include <limits>

void print(const std::string& str)
{
	std::cout << str << '\n';
}

void print_except(const std::exception& e)
{
	std::cout << "Error: " << e.what() << '\n';
}

static void trim_start(std::string& str, const std::string sep)
{
	std::string::size_type s = str.find_first_not_of(sep);
	if (s == std::string::npos)
		throw std::runtime_error("unexpected empty string");
	str = str.erase(0, s);
}

static void trim_end(std::string& str, const std::string sep)
{
	std::string::size_type s = str.find_last_not_of(sep) + 1;
	if (s == std::string::npos)
		throw std::runtime_error("unexpected empty string");
	str = str.erase(s);
}

std::string& trim(std::string& str, const std::string sep)
{
	trim_start(str, sep);
	trim_end(str, sep);
	return(str);
}

unsigned int to_uint(const std::string& str)
{
	if (str.at(0) == '-')
		throw std::runtime_error("invalid negative digit string");
	char* end = NULL;
	errno = 0;
	unsigned long ul = std::strtoul(str.c_str(), &end, 10);
	DEBUG(str);
	DEBUG(ul);
	if (errno || ul > std::numeric_limits<unsigned int>::max()) {
		errno = 0;
		throw std::runtime_error("out of range digit string");
	} else if (*end) {
		throw std::runtime_error("invalid digit string");
	}
	return (static_cast<unsigned int>(ul));
}

double to_pos_double(const std::string& str)
{
	char* end = NULL;
	errno = 0;
	double d = std::strtod(str.c_str(), &end);
	DEBUG(d);
	if (errno == ERANGE || d < 0 || d > std::numeric_limits<double>::max()) {
		errno = 0;
		throw std::runtime_error("out of range digit string");
	} else if (*end) {
		throw std::runtime_error("invalid digit string");
	}
	return (d);
}
