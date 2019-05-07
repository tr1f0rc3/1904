#pragma once

#include "pch.h"
#include "PtrBasedDataStructure.h"

template <typename T>
class Stack final : public PtrBasedDataStructure<T>
{
public:
	void Push(T NodeValue);
	T Top();
	T Pop();
};



template <typename T>
void Stack<T>::Push(T NodeValue)
{
	PtrBasedDataStructure<T>::AddNode(NodeValue);
}

template <typename T>

T Stack<T>::Top()
{
	if (PtrBasedDataStructure<T>::Tail == nullptr)
	{
		std::cout << "empty. NULL would be returned" << std::endl;
		return NULL;
	}
	else
	{
		return PtrBasedDataStructure<T>::Tail->Value;
	}
}

template <typename T>
T Stack<T>::Pop()
{
	if (PtrBasedDataStructure<T>::Tail == nullptr)
	{
		std::cout << "empty. NULL would be returned" << std::endl;
		return NULL;
	}
	else
	{
		T ToBePopped = PtrBasedDataStructure<T>::Tail->Value;
		PtrBasedDataStructure<T>::DeleteTail();
		return ToBePopped;
	}
}





