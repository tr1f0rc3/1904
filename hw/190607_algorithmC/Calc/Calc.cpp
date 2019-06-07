#include "Calc.h"
#include <cassert>


std::string Calc::postfix1() {
	while (!stk.empty()) stk.pop();
	std::string dst;
	bool isdigitFlag(false);
	for (char elem : src) {
		if (isdigit(elem)) {
			dst += stk.top();
			stk.pop();
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

std::string Calc::pf2() {
	return "";
}
