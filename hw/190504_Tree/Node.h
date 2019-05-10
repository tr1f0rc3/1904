#pragma once

template <typename T>
struct Node
{
public:
	T Value;
	const Node* GetParent() const;
	const Node* GetLeftChild() const;
	const Node* GetRightChild() const;

	Node<T>* Parent = nullptr;
	Node<T>* LeftChild = nullptr;
	Node<T>* RightChild = nullptr;
};

template <typename T>
inline const Node<T>* Node<T>::GetLeftChild() const
{
	if (LeftChild == nullptr)
	{
		std::cout << "No left child" << std::endl;
		return nullptr;
	}
	else
	{
		return LeftChild;
	}
}

template <typename T>
inline const Node<T>* Node<T>::GetParent() const
{
	if (Parent == nullptr)
	{
		std::cout << "No left child" << std::endl;
		return nullptr;
	}
	else
	{
		return Parent;
	}
}

template <typename T>
inline const Node<T>* Node<T>::GetRightChild() const
{
	if (RightChild == nullptr)
	{
		std::cout << "No right child" << std::endl;
		return nullptr;
	}
	else
	{
		return RightChild;
	}
}
