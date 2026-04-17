#ifndef DATABASE_HPP
# define DATABASE_HPP

# include "Date.hpp"
# include <map>

class DataBase {
public:
	class Illformed : public std::runtime_error {
	public:
		Illformed(const std::string& msg);
	};
	class OutOfRange : public std::runtime_error {
	public:
		OutOfRange(const std::string& msg);
	};

	DataBase();
	DataBase(
		const std::string& file,
		const char& sep,
		const std::string& first,
		const std::string& second);
	DataBase(const DataBase& src);
	~DataBase();
	DataBase& operator=(const DataBase& src);

	std::pair<Date, double> at(const Date& date);
	// static void check_header(std::ifstream& ifs, const char& sep);
	static void check_header(
			std::ifstream& ifs,
			const char& sep,
			const std::string& first,
			const std::string& second);
	static std::pair<Date, double> parse_line(
										const std::string& line,
										const char& sep);
	// void parse_db(const std::string& file, const char& sep);
	void parse_db(
			const std::string& file,
			const char& sep,
			const std::string& first,
			const std::string& second);
private:
	std::map<Date, double> m_db;
	static std::string& trim(std::string& str, const char& sep);
	static double to_pos_double(const std::string& str);
};

#endif
