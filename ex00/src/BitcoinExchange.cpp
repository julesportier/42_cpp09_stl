#include "utils.hpp"
#include "BitcoinExchange.hpp"
#include "DataBase.hpp"
#include <fstream>

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
BitcoinExchange::BitcoinExchange()
{}

BitcoinExchange::BitcoinExchange(const char* filename)
{
	parse_db(filename, ',');
}

BitcoinExchange::BitcoinExchange(const BitcoinExchange& src)
	: m_db(src.m_db)
{}

BitcoinExchange::~BitcoinExchange() {}

/************
* OPERATORS *
************/
BitcoinExchange& BitcoinExchange::operator=(const BitcoinExchange& src)
{
	if (this != &src) {
		m_db = src.m_db;
	}
	return (*this);
}

/**********
* METHODS *
**********/
void BitcoinExchange::values(const std::string& filename)
{
	std::ifstream dates(filename.c_str());
	if (!dates.is_open())
		throw std::runtime_error("opening input file failed");
	DataBase::check_header(dates, '|');
	for (std::string line; std::getline(dates, line); ) {
		std::pair<Date, double> p;
		try {
			p = DataBase::parse_line(line, '|');
		} catch (const DataBase::Illformed& e) {
			print_except(e);
			continue ;
		}
		if (p.second > 1000) {
			print("Error: too large a number");
			continue ;
		}
		try {
			std::pair<Date, double> val = m_db.at(p.first);
			std::cout << val.first << " => " << p.second
				<< " = " << p.second * val.second
				<< '\n';
		} catch (const DataBase::OutOfRange& e) {
			std::cout << "Error: " << e.what() << " (" << p.first << ")\n";
		}
	}
}

void BitcoinExchange::parse_db(const std::string& file, const char& sep)
{
	DataBase db(file, sep);
	m_db = db;
}
