#include "Date.hpp"
#include "utils.hpp"
#include <vector>
#include <sstream>

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
Date::Date()
{
	m_year = 0;
	m_month = 0;
	m_day = 0;
}

Date::Date(const std::string& date)
{
	set(date);
}

Date::Date(const Date& src)
{
	m_year = src.m_year;
	m_month = src.m_month;
	m_day = src.m_day;
}

Date::~Date() {}

/************
* OPERATORS *
************/
Date& Date::operator=(const Date& src)
{
	if (&src != this) {
		m_year = src.m_year;
		m_month = src.m_month;
		m_day = src.m_day;
	}
	return (*this);
}

bool Date::operator==(const Date& rhs) const
{
	return (m_year == rhs.m_year
				&& m_month == rhs.m_month
				&& m_day == rhs.m_day);
}

bool Date::operator!=(const Date& rhs) const
{
	return (!(*this == rhs));
}

bool Date::operator>(const Date& rhs) const
{
	if (m_year > rhs.m_year) {
		return (true);
	} else if (m_year == rhs.m_year) {
		if (m_month > rhs.m_month)
			return (true);
		else if (m_month == rhs.m_month && m_day > rhs.m_day)
			return (true);
	}
	return (false);
}

bool Date::operator<(const Date& rhs) const
{
	return (rhs > *this);
}

bool Date::operator<=(const Date& rhs) const
{
	return (!(*this > rhs));
}

bool Date::operator>=(const Date& rhs) const
{
	return (!(*this < rhs));
}

/**********
* GETTERS *
**********/
const unsigned int& Date::year() const { return (m_year); }

const unsigned int& Date::month() const { return (m_month); }

const unsigned int& Date::day() const { return (m_day); }

/**********
* SETTERS *
**********/
void Date::set(const std::string& date)
{
	std::vector<std::string> tokens;
	std::stringstream ss;
	ss << date;
	for (std::string tok; std::getline(ss, tok, '-'); ) {
		tokens.push_back(tok);
	}
	if (tokens.size() != 3)
		throw std::runtime_error("invalid date format");
	if (tokens.at(0).size() != 4
			|| tokens.at(1).size() != 2
			|| tokens.at(2).size() != 2)
		throw std::runtime_error("invalid date format");
	year(to_uint(tokens.at(0)));
	// DEBUG(m_year);
	month(to_uint(tokens.at(1)));
	// DEBUG(m_month);
	day(to_uint(tokens.at(1)));
	// DEBUG(m_day);
}

void Date::year(unsigned int y)
{
	if (y < 1582)
		throw std::runtime_error("date before gregorian calendar adoption!");
	m_year = y;
}

void Date::month(unsigned int m)
{
	if (m > 12 || m < 1)
		throw std::runtime_error("invalid month format");
	if (m_year == 1582 && m < 10)
		throw std::runtime_error("date before gregorian calendar adoption!");
	m_month = m;
}

void Date::day(unsigned int d)
{
	if (d < 1 || d > 31)
		throw std::runtime_error("invalid day format");
	if (m_year == 1582 && m_month == 10 && d < 15)
		throw std::runtime_error("date before gregorian calendar adoption!");
	unsigned int day_nbr = 0;
	switch (m_month) {
		case 2:
			day_nbr = 28;
			if (m_year % 4 == 0 && (m_year % 100 != 0 || m_year % 400 == 0))
				day_nbr = 29;
			break;
		case 4:
		case 6:
		case 9:
		case 11:
			day_nbr = 30;
			break;
		default:
			day_nbr = 31;
			break;
	}
	if (d > day_nbr)
		throw std::runtime_error("invalid day format (check leap years)");
	m_day = d;
}

/**********
* METHODS *
**********/
