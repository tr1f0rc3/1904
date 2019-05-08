#pragma once


#pragma once

#include "pch.h"
#include "PtrBasedDataStructure.h"

template <typename T>
class Queue final : public PtrBasedDataStructure<T>
{
public:
	void Push(T NodeValue);
	T Front();
	T Pop();
};

template <typename T>
void Queue<T>::Push(T NodeValue)
{
	PtrBasedDataStructure<T>::AddNode(NodeValue);
}

template <typename T>
T Queue<T>::Front()
{
	if (PtrBasedDataStructure<T>::Head == nullptr)
	{
		std::cout << "empty. NULL would be returned" << std::endl;
		return NULL;
	}
	else
	{
		return PtrBasedDataStructure<T>::Head->Value;
	}
}

template <typename T>
T Queue<T>::Pop()
{
	if (PtrBasedDataStructure<T>::Head == nullptr)
	{
		std::cout << "empty. NULL would be returned" << std::endl;
		return NULL;
	}
	else
	{
		T ToBePopped = PtrBasedDataStructure<T>::Head->Value;
		PtrBasedDataStructure<T>::DeleteHead();
		return ToBePopped;
	}
}





