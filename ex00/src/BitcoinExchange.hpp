#ifndef BITCOINEXCHANGE_HPP
# define BITCOINEXCHANGE_HPP

# include "DataBase.hpp"
# include <string>

class BitcoinExchange {
public:
	BitcoinExchange();
	BitcoinExchange(const char* filename);
	BitcoinExchange(const BitcoinExchange& src);
	~BitcoinExchange();
	BitcoinExchange& operator=(const BitcoinExchange& src);
	void values(const std::string& filename);
private:
	void parse_db(const std::string& file, const char& sep = ',');
	DataBase m_db;
};

#endif
