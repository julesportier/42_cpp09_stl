#include "DataBase.hpp"
#include <limits>
#include <errno.h>
#include <cstdlib>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <vector>

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
DataBase::DataBase() {}

DataBase::DataBase(const std::string& file, const char& sep)
{
	parse_db(file, sep);
}

DataBase::DataBase(const DataBase& src) : m_db(src.m_db) {}

DataBase::~DataBase() {}

/************
* OPERATORS *
************/
DataBase& DataBase::operator=(const DataBase& src)
{
	if (&src != this) {
		m_db = src.m_db;
	}
	return (*this);
}

/*************
* EXCEPTIONS *
*************/
DataBase::Illformed::Illformed(const std::string& msg)
	: std::runtime_error(msg)
{}

DataBase::OutOfRange::OutOfRange(const std::string& msg)
	: std::runtime_error(msg)
{}

/**********
* GETTERS *
**********/
std::pair<Date, double> DataBase::at(const Date& date)
{
	if (m_db.count(date) == 1) {
		return (std::pair<Date, double>(date, m_db.at(date)));
	} else {
		std::map<Date, double>::const_reverse_iterator it(m_db.rbegin());
		while (it != m_db.rend()) {
			if (it->first <= date)
				return (*it);
			++it;
		}
		throw OutOfRange("Date outside database");
	}
}

/**********
* SETTERS *
**********/

/**********
* METHODS *
**********/
void DataBase::check_header(std::ifstream& ifs, const char& sep)
{
	std::string line;
	std::getline(ifs, line);
	std::stringstream line_s;
	line_s << line;
	if (!line_s.good())
		throw std::runtime_error("reading database failed");
	int i = 0;
	for ( ; std::getline(line_s, line, sep) && i < 2; ++i) {}
	if (i == 0)
		throw Illformed("empty database");
	if (i != 2)
		throw Illformed("invalid columns number in database");
}

std::pair<Date, double> DataBase::parse_line(
							const std::string& line,
							const char& sep)
{
	std::stringstream line_s(line);
	line_s << line;
	if (!line_s.good())
		throw std::runtime_error("reading database failed");

	std::vector<std::string> tokens;
	for (std::string tok; std::getline(line_s, tok, sep); ) {
		tokens.push_back(tok);
	}
	if (tokens.size() != 2)
		throw Illformed("invalid columns number in database");

	try {
		const std::string d = trim(tokens[0], ' ');
		const Date date(d);

		const std::string v = trim(tokens[1], ' ');
		const double value = to_pos_double(v);

		return(std::pair<Date, double>(date, value));
	} catch (const Date::Illformed& e) {
		throw Illformed(e.what());
	}
}

void DataBase::parse_db(const std::string& file, const char& sep)
{
	std::ifstream ifs(file.c_str());
	if (!ifs.is_open())
		throw std::runtime_error("opening database failed");

	check_header(ifs, sep);

	for (std::string line; std::getline(ifs, line); ) {
		std::pair<Date, double> p(parse_line(line, sep));
		m_db.insert(p);
	}
	if (m_db.size() == 0)
		throw Illformed("only header in database");
}

std::string& DataBase::trim(std::string& str, const char& sep)
{
	std::string::size_type s = str.find_first_not_of(sep);
	if (s == std::string::npos)
		throw Illformed("unexpected empty string");
	str = str.erase(0, s);

	s = str.find_last_not_of(sep) + 1;
	if (s == std::string::npos)
		throw Illformed("unexpected empty string");
	str = str.erase(s);

	return(str);
}

double DataBase::to_pos_double(const std::string& str)
{
	char* end = NULL;
	errno = 0;
	double d = std::strtod(str.c_str(), &end);
	if (errno == ERANGE || d > std::numeric_limits<double>::max()) {
		errno = 0;
		throw Illformed("out of range number");
	} else if (d < 0) {
		throw Illformed("negative number");
	} else if (*end) {
		throw Illformed("invalid number");
	}
	return (d);
}
