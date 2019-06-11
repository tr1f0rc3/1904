#include "Calc.h"
#include <cassert>


std::string Calc::postfix1() {
	while (!stk.empty()) stk.pop();
	std::string dst;
	bool isdigitFlag(false);
	for (char elem : src) {
		if (isdigit(elem)) {
			dst += elem;
			isdigitFlag = true;
		}
		else {
			if (isdigitFlag) {
				dst += ' ';
				isdigitFlag = false;
			}
			if (elem == ')') {
				assert(!stk.empty());
				dst += stk.top();
				stk.pop();
				dst += ' ';
			}
			else if (elem == '+' || elem == '-' || elem == '*' || elem == '/') {
				stk.push(elem);
			}
			else {
			}
		}
	}
	return dst;
}

std::string Calc::postfix2() {
	while (!stk.empty()) stk.pop();
	std::string dst;
	bool isdigitFlag(false);
	for (char elem : src) {
		if (isdigit(elem)) {
			dst += elem;
			isdigitFlag = true;
		}
		else {
			if (isdigitFlag) {
				dst += ' ';
				isdigitFlag = false;
			}
			if (elem == '(') {
				stk.push(elem);
			}
			else if (elem == ')') {
				while (stk.top() != '(') {
					dst += popAndSpace();
				}
				stk.pop();
			}
			else if (isOperator(elem)) {
				while (!stk.empty() && precednece(stk.top()) >= precednece(elem)) {
					dst += popAndSpace();
				}
				stk.push(elem);
			}
			else {}
		}
	}
	while (!stk.empty()) {
		dst += popAndSpace();
	}
	return dst;
}

bool Calc::isOperator(char _char) const {
	return  (_char == '+' || _char == '-' || _char == '*' || _char == '/');
}

int Calc::precednece(char _char) const {
	if (_char == '(') return 0;
	else if (_char == '+' || _char == '-') return 1;
	else if (_char == '*' || _char == '/') return 2;
	else return 3;
}

std::string Calc::popAndSpace() {
	assert(!stk.empty());
	std::string toReturn;
	toReturn += stk.top();
	stk.pop();
	toReturn += ' ';
	return toReturn;
}

int Calc::eval(std::string toEval) {
	int i(0);
	while (!iStk.empty()) iStk.pop();
	std::string dst;
	bool isdigitFlag(false);
	for (char elem : toEval) {
		if (isdigit(elem)) {
			i = i * 10 + static_cast<int>(elem - '0');
			isdigitFlag = true;
		}
		else {
			if (isdigitFlag) {
				iStk.push(i);
				i = 0;
				isdigitFlag = false;
			}
			if (elem == '+' || elem == '-' || elem == '*' || elem == '/') {
				int rhs = iStk.top();
				iStk.pop();
				int lhs = iStk.top();
				iStk.pop();
				switch (elem) {
				case '+': {
					iStk.push(lhs + rhs);
					break;
				}
				case '-': {
					iStk.push(lhs - rhs);
					break;
				}
				case '*': {
					iStk.push(lhs * rhs);
					break;
				}
				case '/': {
					iStk.push(lhs / rhs);
					break;
				}
				default:
					assert(0);
					break;
				}
			}
			else {}
		}
	}
	int toReturn = iStk.top();
	iStk.pop();
	return toReturn;
}
