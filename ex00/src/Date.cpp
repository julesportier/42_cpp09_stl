#include "Date.hpp"
#include <sstream>
#include <cstdlib>
#include <errno.h>
#include <limits>

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
Date::Date()
	: m_year(0)
	, m_month(0)
	, m_day(0)
{}

Date::Date(const std::string& date)
{
	set(date);
}

Date::Date(const Date& src)
	: m_year(src.m_year)
	, m_month(src.m_month)
	, m_day(src.m_day)
{}

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

std::ostream& operator<<(std::ostream& os, const Date& date)
{
	os << date.year() << '-';
	if (date.month() < 10)
		os << '0';
	os << date.month() << '-';
	if (date.day() < 10)
		os << '0';
	os << date.day();
	return (os);
}

/*************
* EXCEPTIONS *
*************/
Date::Illformed::Illformed(const std::string& msg)
	: std::runtime_error(msg)
{};

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
	std::stringstream ss;
	ss << date;

	std::string y;
	if (!std::getline(ss, y, '-') || y.size() != 4)
		throw Illformed("invalid date format: '" + date + '\'');

	std::string m;
	if (!std::getline(ss, m, '-') || m.size() != 2)
		throw Illformed("invalid date format: '" + date + '\'');

	std::string d;
	if (!std::getline(ss, d, '-') || d.size() != 2 || !ss.eof())
		throw Illformed("invalid date format: '" + date + '\'');

	try {
		year(to_uint(y));
		month(to_uint(m));
		day(to_uint(d));
	} catch (const Illformed& e) {
		throw Illformed(std::string(e.what()) + ": '" + date + '\'');
	}
}

void Date::year(unsigned int y)
{
	if (y < 1582)
		throw Illformed("date before gregorian calendar adoption!");
	m_year = y;
}

void Date::month(unsigned int m)
{
	if (m > 12 || m < 1)
		throw Illformed("invalid date format");
	if (m_year == 1582 && m < 10)
		throw Illformed("date before gregorian calendar adoption!");
	m_month = m;
}

void Date::day(unsigned int d)
{
	if (d < 1 || d > 31)
		throw Illformed("invalid day format");
	if (m_year == 1582 && m_month == 10 && d < 15)
		throw Illformed("date before gregorian calendar adoption!");
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
		throw Illformed("invalid day format");
	m_day = d;
}

/**********
* METHODS *
**********/
unsigned int Date::to_uint(const std::string& str)
{
	if (str.at(0) == '-')
		throw Illformed("invalid negative number");
	char* end = NULL;
	errno = 0;
	unsigned long ul = std::strtoul(str.c_str(), &end, 10);
	if (errno || ul > std::numeric_limits<unsigned int>::max()) {
		errno = 0;
		throw Illformed("out of range number");
	} else if (*end) {
		throw Illformed("unexpected character in number");
	}
	return (static_cast<unsigned int>(ul));
}
