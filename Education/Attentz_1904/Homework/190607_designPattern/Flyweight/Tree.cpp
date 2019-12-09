#include "pch.h"
#include "Tree.h"

Leaf* Tree::leaf = nullptr;
Root* Tree::root = nullptr;

Tree::Tree()
{
	if (leaf == nullptr) {
		leaf = new Leaf();
	}
	if (root == nullptr) {
		root = new Root();
	}
}


Tree::~Tree()
{
	delete leaf;
	delete root;
}


