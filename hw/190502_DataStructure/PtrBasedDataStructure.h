#pragma once

#include "pch.h"
#include "Node.h"

template <typename T>
class PtrBasedDataStructure
{
public:
	virtual ~PtrBasedDataStructure();
	void AddNode(T);

protected:
	bool DeleteHead();
	bool DeleteTail();

	Node<T>* Head = nullptr;
	Node<T>* Tail = nullptr;

};

template <typename T>
PtrBasedDataStructure<T>::~PtrBasedDataStructure()
{
	Node<T>* FrontierNode(Head);
	Node<T>* FollowingNode(Head);

	while (FollowingNode != nullptr)
	{
		FrontierNode = FollowingNode->NextNodePtr;
		delete FollowingNode;
		FollowingNode = FrontierNode;
	}
}

template <typename T>
void PtrBasedDataStructure<T>::AddNode(T NodeValueInput)
{
	Node<T>* NewNode = new Node<T>;
	NewNode->Value = NodeValueInput;

	if (Head == nullptr)
	{
		Head = NewNode;
		Head->PrevNodePtr = nullptr;
	}
	else
	{
		NewNode->PrevNodePtr = Tail;
		Tail->NextNodePtr = NewNode;
	}
	NewNode->NextNodePtr = nullptr;
	Tail = NewNode;
}

template <typename T>
bool PtrBasedDataStructure<T>::DeleteHead()
{
	if (Head == nullptr)
	{
		return false;
	}
	Node<T>* ToBeDeleted = Head;
	if (Head->GetNext() != nullptr)
	{
		Head->NextNodePtr->PrevNodePtr = nullptr;
		Head = Head->NextNodePtr;
	}
	else
	{
		Head = nullptr;
		Tail = nullptr;
	}
	delete ToBeDeleted;
}

template <typename T>
bool PtrBasedDataStructure<T>::DeleteTail()
{
	if (Tail == nullptr)
	{
		return false;
	}
	Node<T>* ToBeDeleted = Tail;
	if (Tail->GetPrev() != nullptr)
	{
		Tail->PrevNodePtr->NextNodePtr = nullptr;
		Tail = Tail->PrevNodePtr;
	}
	else
	{
		Head = nullptr;
		Tail = nullptr;
	}
	delete ToBeDeleted;
}
