#pragma once
template <typename T>
class SP
{
public:
	SP(T* _ref);
	~SP();

	T* operator->();
	T operator*();


private:
	T* ref;
};

//#include "SP.cpp"

template <typename T>
SP<T>::SP(T* _ref) : ref(_ref) { }

template <typename T>
SP<T>::~SP() {
	if (ref != nullptr) {
		delete ref;
	}
}

template <typename T>
T* SP<T>::operator->() {
	return ref;
}

template <typename T>
T SP<T>::operator*() {
	return *ref;
}

struct ab {
	ab(int _a, int _b) : a(_a), b(_b) {}
	int a;
	int b;
};