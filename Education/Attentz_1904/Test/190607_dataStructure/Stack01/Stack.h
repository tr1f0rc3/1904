#pragma once

#include "LinkedList.h"

template <typename T>
class Stack
{
public:
	Stack() {
		head = new Node<T>();
		tail = new Node<T>();
		head->next = tail;
	};
	virtual ~Stack() {
		Node<T>* toBeDeleted = head->next;
		Node<T>* nextToBeDeleted;
		while (toBeDeleted != tail) {
			nextToBeDeleted = toBeDeleted->next;
			delete toBeDeleted;
			toBeDeleted = nextToBeDeleted;
		}
		delete head;
		head = nullptr;
		delete tail;
		tail = nullptr;
	}

	Node<T>* head;
	Node<T>* tail;

	void push(T _value) {
		auto newNode = new Node<T>(_value);
		newNode->next = head->next;
		head->next = newNode;
	}

	T pop() {
		if (head->next == tail) {
			std::cout << "the stack is empty" << std::endl;
			return T();
		}
		else {
			auto toBePopped = head->next;
			T toReturn = toBePopped->data;
			head->next = toBePopped->next;
			delete toBePopped;
			return toReturn;
		}
	}
};

