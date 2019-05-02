#pragma once

#include "pch.h"
#include "Node.h"

template <typename T>
class Stack
{
public:
	~Stack();
	std::vector<T> Data;

	void Push(T);
	T Top();
	T Pop();
};

template <typename T>
Stack<T>::~Stack()
{
	while (Data.empty())
	{
		Pop();
	}
}

template <typename T>
void Stack<T>::Push(T NodeValueInput)
{
	Data.emplace_back(NodeValueInput);
}

template <typename T>

T Stack<T>::Top()
{
	if (Data.empty())
	{
		std::cout << "empty. NULL would be returned" << std::endl;
		return NULL;
	}
	else
	{
		return *(Data.end() - 1);
	}
}

template <typename T>
T Stack<T>::Pop()
{
	if (Data.empty())
	{
		std::cout << "empty. NULL would be returned" << std::endl;
		return NULL;
	}
	else
	{
		T ToBePopped = *(Data.end() - 1);
		Data.erase(Data.end() - 1);
		return ToBePopped;
	}
}





