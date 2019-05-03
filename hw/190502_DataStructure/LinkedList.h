#pragma once

#include "pch.h"
#include "PtrBasedDataStructure.h"

template <typename T>
class LinkedList final : public PtrBasedDataStructure<T>
{
public:
	void PrintAll();
	Node<T>* FindNode(T);
	bool DeleteNode(T);
};

template <typename T>
void LinkedList<T>::PrintAll()
{
	Node<T>* TraversingNode(PtrBasedDataStructure<T>::Head);
	while (TraversingNode != nullptr)
	{
		std::cout << TraversingNode->Value << ' ';
		TraversingNode = TraversingNode->NextNodePtr;
	}
	std::cout << std::endl;
}

template <typename T>
Node<T>* LinkedList<T>::FindNode(T NodeValueInput)
{
	Node<T>* TraversingNode(PtrBasedDataStructure<T>::Head);
	while (TraversingNode != nullptr)
	{
		if (TraversingNode->Value == NodeValueInput)
		{
			return TraversingNode;
		}
		else
		{
			TraversingNode = TraversingNode->NextNodePtr;
		}
	}
	return nullptr;
}

template <typename T>
bool LinkedList<T>::DeleteNode(T NodeValueInput)
{
	Node<T>* ToBeDeletedNode = FindNode(NodeValueInput);
	if (ToBeDeletedNode == nullptr)
	{
		return false;
	}
	else if (ToBeDeletedNode == PtrBasedDataStructure<T>::Head && ToBeDeletedNode == PtrBasedDataStructure<T>::Tail)
	{
		PtrBasedDataStructure<T>::Head = nullptr;
		PtrBasedDataStructure<T>::Tail = nullptr;
	}
	else if (ToBeDeletedNode == PtrBasedDataStructure<T>::Head)
	{
		ToBeDeletedNode->NextNodePtr->PrevNodePtr = nullptr;
		PtrBasedDataStructure<T>::Head = ToBeDeletedNode->NextNodePtr;
	}
	else if (ToBeDeletedNode == PtrBasedDataStructure<T>::Tail)
	{
		ToBeDeletedNode->PrevNodePtr->NextNodePtr = nullptr;
		PtrBasedDataStructure<T>::Tail = ToBeDeletedNode->PrevNodePtr;
	}
	else
	{
		ToBeDeletedNode->PrevNodePtr->NextNodePtr = ToBeDeletedNode->NextNodePtr;
		ToBeDeletedNode->NextNodePtr->PrevNodePtr = ToBeDeletedNode->PrevNodePtr;
	}
	delete ToBeDeletedNode;
	return true;
}

