#include "RPN.h"
#include <limits>
#include <stdexcept>
#include <sstream>

/**************************
* CONSTRUCTORS/DESTRUCTOR *
**************************/
RPN::RPN() {}

RPN::RPN(const std::string& exp)
{
	expression(exp);
}

RPN::RPN(const RPN& src)
	: m_expression(src.m_expression)
{}

RPN::~RPN() {}

RPN& RPN::operator=(const RPN& src)
{
	if (&src != this) {
		m_expression = src.m_expression;
	}
	return (*this);
}

/**********
* METHODS *
**********/
long RPN::compute() const
{
	if (m_expression.size() == 0)
		throw std::runtime_error("can't compute empty expression");
	std::vector<char>::const_iterator it(m_expression.begin());
	std::stack<long> digits;
	for ( ; it != m_expression.end(); ++it) {
		if (is_digit(*it))
			digits.push(*it - '0');
		else
			apply(*it, digits);
	}
	if (digits.size() != 1)
		throw std::runtime_error("invalid expression");
	return (digits.top());
}

void RPN::apply(const char& op, std::stack<long>& stack) const
{
	if (stack.size() < 2)
		throw std::runtime_error("invalid expression");
	long second = stack.top();
	stack.pop();
	long first = stack.top();
	stack.pop();
 	switch (op) {
	case '+':
		if ((second > 0 && first > std::numeric_limits<long>::max() - second)
				|| (second < 0 && first < std::numeric_limits<long>::min() - second))
			throw std::runtime_error("overflow");
		stack.push(first + second);
		break;
	case '-':
		if ((second < 0 && first > std::numeric_limits<long>::max() + second)
				|| (second > 0 && first < std::numeric_limits<long>::min() + second))
			throw std::runtime_error("overflow");
		stack.push(first - second);
		break;
	case '*':
		if ((second != 0 && first > std::numeric_limits<long>::max() / second)
				|| (second != 0 && first < std::numeric_limits<long>::min() / second)
				|| (first == -1 && second == std::numeric_limits<long>::min())
				|| (second == -1 && first == std::numeric_limits<long>::min()))
			throw std::runtime_error("overflow");
		stack.push(first * second);
		break;
	case '/':
		if (second == 0
				|| (second == -1 && first == std::numeric_limits<long>::min()))
			throw std::runtime_error("division by zero");
		stack.push(first / second);
		break;
	}
}

bool RPN::is_digit(const char& c) const
{
	return (c >= '0' && c <= '9');
}

bool RPN::is_operator(const char& c) const
{
	 return (c == '+' || c == '-' || c == '*' || c == '/');
}

/**********
* GETTERS *
**********/
const std::vector<char> RPN::expression() const
{
	return (m_expression);
}

/**********
* SETTERS *
**********/
void RPN::expression(const std::string& exp)
{
	std::stringstream ss;
	ss << exp;
	for (std::string token; getline(ss, token, ' '); ) {
		if (token.size() != 1)
			throw std::runtime_error("invalid token in expression");
		const char tok = token[0];
		if (!is_digit(tok) && !is_operator(tok))
			throw std::runtime_error("invalid token in expression");
		m_expression.push_back(tok);
	}
}
