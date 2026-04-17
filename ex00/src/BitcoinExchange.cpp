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
	DataBase::check_header(dates, '|', "date", "value");
	for (std::string line; std::getline(dates, line); ) {
		std::pair<Date, double> input;
		try {
			input = DataBase::parse_line(line, '|');
			if (input.second > 1000) {
				print("Error: too large a number");
				continue;
			}
			std::pair<Date, double> db_entry = m_db.at(input.first);
			std::cout << db_entry.first << " => " << input.second
				<< " = " << input.second * db_entry.second << '\n';
		} catch (const DataBase::Illformed& e) {
			print_except(e);
		} catch (const DataBase::OutOfRange& e) {
			std::cout << "Error: " << e.what() << " (" << input.first << ")\n";
		}
	}
}

void BitcoinExchange::parse_db(const std::string& file, const char& sep)
{
	DataBase db(file, sep, "date", "exchange_rate");
	m_db = db;
}
