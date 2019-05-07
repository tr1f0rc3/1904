// ConsoleApplication3.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//

#include "pch.h"


#include <iostream>
#include <cassert>
#include <vld.h>
using namespace std;

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

template <typename T>
class myqueue {
public:
	myqueue(T _maxSize) : maxSize(_maxSize), arr((_maxSize > 0) ? new T[_maxSize] : nullptr) {	}
	~myqueue() {
		delete[] arr;
		arr = nullptr;
	}

	bool push_back(const int& _elem) {
		if (curr == maxSize) {
			cout << "full" << endl;
			return false;
		}
		else {
			arr[curr++] = _elem;
			return true;
		}
	}
	bool pop() {
		if (curr == 0) {
			cout << "empty" << endl;
			return false;
		}
		else if (curr == 1) { arr[--curr] = 0; }

		else {
			for (int i = 0; i != curr - 1; ++i) {
				arr[i] = arr[i + 1];
			}
			arr[--curr] = 0;
		}
	}
	int top() {
		if (curr == 0) {
			cout << "empty" << endl;
			return -1;
		}
		else {
			return arr[0];
		}
	}

	T* arr = nullptr;
	int maxSize = 1;
	int curr = 0;
};

template <typename T>
class mystack {
public:
	mystack(int _maxSize) : maxSize(_maxSize), arr((_maxSize > 0) ? new T[_maxSize] : nullptr) {}
	~mystack() {
		delete[] arr;
		arr = nullptr;
	}

	bool push_back(const int& _elem) {
		if (curr == maxSize) {
			cout << "full" << endl;
			return false;
		}
		else {
			arr[curr++] = _elem;
			return true;
		}
	}
	bool pop() {
		if (curr == 0) {
			cout << "empty" << endl;
			return false;
		}
		else {
			arr[--curr] = 0;
			return true;
		}
	}
	int top() {
		if (curr == 0) {
			cout << "empty" << endl;
			return -1;
		}
		else {
			return arr[curr - 1];
		}
	}

	T* arr = nullptr;
	int maxSize = 1;
	int curr = 0;
};


int main() {

	cout << "myqueue" << endl;
	enum { _maxSize = 3, };


	myqueue<int> qoo(_maxSize);
	qoo.top();
	qoo.pop();
	for (int i = 0; i != _maxSize; ++i) {
		qoo.push_back(i);
	}

	for (int i = 0; i != _maxSize; ++i) {
		cout << qoo.top() << ' ';
		qoo.pop();
	}
	cout << endl;

	cout << "mystack" << endl;



	mystack<int> stk(_maxSize);
	stk.top();
	stk.pop();
	for (int i = 0; i != _maxSize; ++i) {
		stk.push_back(i);
	}
	for (int i = 0; i != _maxSize; ++i) {
		cout << stk.top() << ' ';
		stk.pop();
	}
	cout << endl;

	getchar();

}