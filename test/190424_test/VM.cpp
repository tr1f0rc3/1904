#include "pch.h"
#include "VM.h"

map<int, int> VM::getMenu() const { return menu; }

map<int, int> VM::buy(int _num, int _money) {
	auto it = menu.find(_num);
	if (it == menu.end()) {
		cout << "The code not found" << endl;
	}
	else if (_money < it->second) {
		cout << "insert more money" << endl;
	}
	else {
		map<int, int> change;
		int discri = _money - it->second;
		int temp;
		for (auto it = changeUnit.rbegin(); it != changeUnit.rend(); ++it) {
			int elem = *it;
			if (discri >= elem) {
				change.insert({ elem, (discri / elem) });
				discri %= elem;
			}
		}
		return change;
	}
}
