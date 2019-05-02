#pragma once

template <typename T>
struct Node
{
	T Value;
	const Node* GetNext() const;
	const Node* GetPrev() const;

	Node<T>* NextNodePtr = nullptr;
	Node<T>* PrevNodePtr = nullptr;
};


template <typename T>
inline const Node<T>* Node<T>::GetNext() const
{
	if (NextNodePtr == nullptr)
	{
		std::cout << "this is the tail" << std::endl;
		return nullptr;
	}
	else
	{
		return NextNodePtr;
	}

}

template <typename T>
inline const Node<T>* Node<T>::GetPrev() const
{
	if (PrevNodePtr == nullptr)
	{
		std::cout << "this is the head" << std::endl;
		return nullptr;
	}
	else
	{
		return PrevNodePtr;
	}
}



