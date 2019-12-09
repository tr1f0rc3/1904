#pragma once

template <typename T>
class Node
{
public:
	Node() = default;
	Node(T _data) : data(_data) {};
	virtual ~Node() {}

	T data;
	Node* next = nullptr;
};

