#pragma once
#include "pch.h"
class Tree
{
public:
	Tree();
	~Tree();

	static class Leaf* leaf;
	static class Root* root;

	int age = 0;

};

class Leaf {
	int vein = 8;
};

class Root {
	int branch = 12;
};



