#include "pch.h"
#include "isPrime.h"

#pragma once

bool isPrime::operator()(int _input) {
	if (primeSet.find(_input) != primeSet.end()) {
		return true;
	}
	assert(_input > 2);
	for (int i = 0; i != 2; ++i) {
		for (auto elem : primeSet) {
			if (MCN(_input, elem) != 1) {
				return false;
			}
		}
		if (i == 0) { buildDB(_input); }
		else {
			primeSet.insert(_input);
			return true;
		}
	}
}

void isPrime::buildDB(int _input) {
	for (int i = 3; i < _input; ++i) {
		if (primeSet.find(i) != primeSet.end()) {
			for (auto elem : primeSet) {
				int theValue = MCN(i, elem);
				if (theValue == 1) {
					primeSet.insert(i);
				}
			}
		}
	}
}

int isPrime::MCN(int lhs, int rhs) {
	assert(lhs != 0 && rhs != 0);
	if (rhs > lhs) {
		swap(lhs, rhs);
	}
	int mcn = 1;
	if (lhs % rhs != 0) {
		mcn = MCN(lhs - rhs, rhs);
	}
	else { mcn = rhs; }
	return mcn;
}
