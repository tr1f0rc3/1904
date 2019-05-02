#pragma once

#include "Node.h"

template <typename T>
class LinkedList
{
public:
	~LinkedList();

	void PrintAll();
	void AddNode(T);
	Node<T>* FindNode(T);
	bool DeleteNode(T);

	Node<T>* Head = nullptr;
	Node<T>* Tail = nullptr;
};

template <typename T>
LinkedList<T>::~LinkedList()
{
	Node<T>* FrontierNode(Head);
	Node<T>* FollowingNode(Head);

	while (FollowingNode != nullptr)
	{
		FrontierNode = FollowingNode->NextNodePtr;
		delete FollowingNode;
		FollowingNode = nullptr;
		FollowingNode = FrontierNode;
	}
}

template <typename T>
void LinkedList<T>::PrintAll()
{
	Node<T>* TraversingNode(Head);
	while (TraversingNode != nullptr)
	{
		std::cout << TraversingNode->Value << ' ';
		TraversingNode = TraversingNode->NextNodePtr;
	}
	std::cout << std::endl;
}


template <typename T>
void LinkedList<T>::AddNode(T NodeValueInput)
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
Node<T>* LinkedList<T>::FindNode(T NodeValueInput)
{
	Node<T>* TraversingNode(Head);
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
	else if (ToBeDeletedNode == Head && ToBeDeletedNode == Tail)
	{
	}
	else if (ToBeDeletedNode == Head)
	{
		ToBeDeletedNode->NextNodePtr->PrevNodePtr = nullptr;
		Head = ToBeDeletedNode->NextNodePtr;
	}
	else if (ToBeDeletedNode == Tail)
	{
		ToBeDeletedNode->PrevNodePtr->NextNodePtr = nullptr;
		Tail = ToBeDeletedNode->PrevNodePtr;
	}
	else
	{
		ToBeDeletedNode->PrevNodePtr->NextNodePtr = ToBeDeletedNode->NextNodePtr;
		ToBeDeletedNode->NextNodePtr->PrevNodePtr = ToBeDeletedNode->PrevNodePtr;
	}
	delete ToBeDeletedNode;
	ToBeDeletedNode = nullptr;
	return true;
}

