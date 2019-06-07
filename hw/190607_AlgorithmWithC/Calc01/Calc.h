#pragma once

#include <string>
#include <stack>

class Calc
{
public:

	std::string postfix1();
	std::string pf2();

	std::string src;
	std::stack<char> stk;

};

