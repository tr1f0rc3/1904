#pragma once

#include <string>
#include <stack>

class Calc
{
public:

	std::string postfix1();
	std::string postfix2();
	int eval(std::string toEval);

	std::string src;

protected:
	bool isOperator(char _char) const;
	int precednece(char _char) const;
	std::string popAndSpace();

	std::stack<char> stk;
	std::stack<int> iStk;


};

