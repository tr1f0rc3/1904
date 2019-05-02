#pragma once
#include "pch.h"
template <typename T>
struct Composite {
	virtual ~Composite() {}
	std::vector<T*> composite;
	void add(T* _toAdd) { composite.emplace_back(_toAdd); }
	void clear() { composite.clear(); }
};