#ifndef RPN_H
# define RPN_H

# include <stack>
# include <string>
# include <vector>

class RPN {
public:
	RPN();
	RPN(const std::string& exp);
	RPN(const RPN& src);
	~RPN();
	RPN& operator=(const RPN& src);

	long compute() const;

	const std::vector<char> expression() const;
	void expression(const std::string& exp);
private:
	std::vector<char> m_expression;
	void apply(const char& op, std::stack<long>& stack) const;
	bool is_digit(const char& c) const;
	bool is_operator(const char& c) const;
};

#endif
