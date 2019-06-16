#pragma once
#include <vector>
#include <iostream>
#include <cassert>


template <typename T>
class Heap
{
public:
	Heap() = default;
	Heap(const std::vector<T>& _data);

	int getSize() const;
	void minHeapify(int idx);
	T extractMin();
	void insert(const T& elem);
	void printAll() const;
	void clearAll();
	bool isSorted() const;

protected:
	std::vector<T> data;
};

template <typename T>
Heap<T>::Heap(const std::vector<T>& _data) : data(_data) {
	int n = data.size() / 2;
	for (int i = 0; i < n; ++i) {
		minHeapify(i);
	}
};

template <typename T>
int Heap<T>::getSize() const {
	return data.size();
}


template <typename T>
void Heap<T>::minHeapify(int idx) {

	assert((size_t)idx < data.size() && idx >= 0);
	int front(idx), follow(idx), toSwap(data[idx]), n(data.size());

	while (follow < n / 2) {
		front = (follow << 1) + 1;
		if (front < n - 1 && data[front] > data[front + 1]) {
			++front;
		}
		if (toSwap <= data[front]) break;
		data[follow] = data[front];
		follow = front;
	}

	data[follow] = toSwap;
}

template <typename T>
T Heap<T>::extractMin() {

	assert(data.size() >= 0);
	int n = data.size();
	auto toReturn(data[0]);

	if (n == 1) {
		data.clear();
	}
	else {
		data[0] = *(data.end() - 1);
		data.erase(data.end() - 1);
		minHeapify(0);
	}

	return toReturn;
}

template <typename T>
void Heap<T>::insert(const T& elem) {
	data.emplace_back(elem);

	int n(data.size() - 1);
	if (n == 0) {
		return;
	}
	else {
		int front((n - 1) / 2), follow(n);
		while (follow != 0 && elem < data[front]) {
			data[follow] = data[front];
			follow = front;
			front = ((front - 1) >> 1);
		}
		data[follow] = elem;
	}
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
		if (data[i] < data[parent]) {
			return false;
		}
	}
	return true;
}
