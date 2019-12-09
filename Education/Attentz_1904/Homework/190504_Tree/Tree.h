#pragma once

template <typename T>
class Node;

template <typename T>
class Tree
{
public:
	Tree() = default;
	Tree(Node<T> RootData) : Root(RootData) {}
	virtual ~Tree();
	
	Node<T>* Root;

protected:
	void DeleteChildrenRecursive(Node<T>* NodeToBeDeleted);
};

template <typename T>
Tree<T>::~Tree()
{
	DeleteChildrenRecursive(Root);
}

template <typename T>
void Tree<T>::DeleteChildrenRecursive(Node<T>* NodeToBeDeleted)
{
	Node<T>* LeftChild = NodeToBeDeleted.GetLeftChild();
	if (LeftChild != nullptr)
	{
		DeleteChildrenRecursive(LeftChild);
	}
	Node<T>* RightChild = NodeToBeDeleted.GetRightChild();
	if (RightChild != nullptr)
	{
		DeleteChildrenRecursive(RightChild);
	}
	delete NodeToBeDeleted;
	NodeToBeDeleted = nullptr;
}

