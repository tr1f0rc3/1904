#pragma once
#include <vector>
#include <iostream>
#include <cassert>
#include <stack>


template <typename T>
class Heap
{
public:
	Heap() = default;
	Heap(bool _isMax) : isMax(_isMax) {};
	Heap(bool _isMax, const std::vector<T>& _data);

	int getSize() const;
	bool resize(size_t _size);
	int heapify(int idx);
	T extract();
	int insert(const T& elem);
	bool makeChild(int childPos, int parentPos, bool isLeft = true);

	const std::vector<T>& getData() const;
	void printAll() const;
	void clearAll();
	bool isSorted() const;

	std::vector<T> data;

protected:
	bool compare(const T& lhs, const T& rhs) const;
	bool doesExist(int idx);

	bool isMax = false;


private:

};

template <typename T>
Heap<T>::Heap(bool _isMax, const std::vector<T>& _data) : isMax(_isMax), data(_data) {
	int n = data.size() / 2;
	for (int i = 0; i < n; ++i) {
		heapify(i);
	}
};

template <typename T>
bool Heap<T>::resize(size_t size) {
	if (data.size() >= size) {
		return false;
	}
	else {
		data.resize(size);
	}
	return true;
}


template <typename T>
const std::vector<T>& Heap<T>::getData() const {
	return data;
}


template <typename T>
bool Heap<T>::compare(const T& lhs, const T& rhs) const {
	if (isMax) {
		return lhs > rhs;
	}
	else {
		return lhs < rhs;
	}
}

template <typename T>
bool Heap<T>::doesExist(int idx) {
	if (data.size() > idx && data[idx] != T()) {
		return true;
	}
	else {
		return false;
	}
}

template <typename T>
bool Heap<T>::makeChild(int childPos, int parentPos, bool isLeft) {
	if (compare(data[childPos], data[parentPos])) {
		return false;
	}

	int oldPrev, oldLeft, oldRight, newPrev, newLeft, newRight;

	if (isLeft) {
		newLeft = (parentPos << 1) + 1;
		if (doesExist(newLeft)) {
			return false;
		}
	}
	else {
		newRight = newLeft + 1;
		if (doesExist(newRight)) {
			return false;
		}
	}

	std::stack<std::pair<int, int>> stk;
	stk.push(std::make_pair(childPos, parentPos));
	while (!stk.empty()) {
		oldPrev = stk.top().first;
		oldLeft = (oldPrev << 1) + 1;
		oldRight = newLeft + 1;
		newPrev = stk.top().second;
		newLeft = (newPrev << 1) + 1;
		newRight = newLeft + 1;
		stk.pop();

		if (!doesExist(oldRight)) {

		}
		if (doesExist(oldRight)) {
			std::swap(data[newRight], data[oldRight]);
			stk.push(std::make_pair(oldRight, newRight));
		}
		if (doesExist(oldLeft)) {
			std::swap(data[newLeft], data[oldLeft]);
			stk.push(std::make_pair(oldLeft, newLeft));
		}
	}

	return true;
}


template <typename T>
int Heap<T>::getSize() const {
	return data.size();
}


template <typename T>
int Heap<T>::heapify(int idx) {

	assert((size_t)idx < data.size() && idx >= 0);
	int front(idx), follow(idx), n(data.size());
	T toSwap(data[idx]);

	while (follow < n / 2) {
		front = (follow << 1) + 1;
		if (front < n - 1 && compare(data[front + 1], data[front])) {
			++front;
		}
		if (compare(toSwap, data[front]) || toSwap == data[front]) break;
		data[follow] = data[front];
		follow = front;
	}

	data[follow] = toSwap;
	return follow;
}

template <typename T>
T Heap<T>::extract() {

	assert(data.size() >= 0);
	int n = data.size();
	auto toReturn(data[0]);

	data[0] = *(data.end() - 1);
	data.erase(data.end() - 1);
	if (!data.empty()) {
		heapify(0);
	}


	return toReturn;
}

template <typename T>
int Heap<T>::insert(const T& elem) {
	data.emplace_back(elem);


	int n(data.size() - 1);
	int front((n - 1) / 2), follow(n);

	while (follow != 0 && compare(elem, data[front])) {
		data[follow] = data[front];
		follow = front;
		front = ((front - 1) >> 1);
	}
	data[follow] = elem;
	return follow;
}

template <typename T>
void Heap<T>::printAll() const {
	if (!data.empty()) {
		for (auto& elem : data) {
			std::cout << elem << ", ";
		}
	}
	std::cout << std::endl;
}

template <typename T>
void Heap<T>::clearAll() {
	data.clear();
}

template <typename T>
bool Heap<T>::isSorted() const {
	int parent, n(data.size() - 1);
	for (int i = n; i > 0; --i) {
		parent = ((i - 1) >> 1);
		if (compare(data[i], data[parent])) {
			return false;
		}
	}
	return true;
}
