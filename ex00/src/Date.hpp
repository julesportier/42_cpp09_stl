#ifndef DATE_HPP
# define DATE_HPP

# include <string>

class Date {
public:
	Date();
	Date(const std::string& date);
	Date(const Date& src);
	~Date();

	Date& operator=(const Date& src);
	bool operator==(const Date& rhs) const;
	bool operator!=(const Date& rhs) const;
	bool operator>(const Date& rhs) const;
	bool operator<(const Date& rhs) const;
	bool operator<=(const Date& rhs) const;
	bool operator>=(const Date& rhs) const;

	void set(const std::string& date);
	const unsigned int& year() const;
	void year(unsigned int y);
	const unsigned int& month() const;
	void month(unsigned int m);
	const unsigned int& day() const;
	void day(unsigned int d);
private:
	unsigned int m_year;
	unsigned int m_month;
	unsigned int m_day;
};

#endif
