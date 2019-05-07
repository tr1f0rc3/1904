#pragma once
class VM
{
public:
	map<int, int> getMenu() const;
	static map<int, int> buy(int _num, int _money);

private:
	inline static map<int, int> menu{ {1, 500}, { 2,300 }, { 3,400 } };
	inline static set<int> changeUnit { 50000, 10000, 1000, 500, 100, 50, 10, 5, 1 };

};

