#pragma once

struct isPrime {
	bool operator()(int _input);
private:
	int MCN(int lhs, int rhs);
	void buildDB(int _input);
	inline static set<int> primeSet{ 1, 2 };
};