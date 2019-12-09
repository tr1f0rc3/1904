// #include "pch.h"
#include <iostream>     // std::cout
#include <ctime>
#include <cstdlib>
#include <bitset>

using namespace std;

using int32 = signed int;
using int8 = signed char;
using uint32 = unsigned int;
using uint8 = unsigned char;

template<typename T>
T Add(const T& a, const T& b)
{
	return T(a + b);
}

template<>
double Add(const double& a, const double& b)
{
	std::cout << "double Add" << std::endl;
	return a + b;
}


enum class eStackException
{
	EmptyBuffer = 0,
};

template<typename T, uint32 N>
class MyStack
{
	static_assert(N <= 32);

public:

	void Push(const T& val);
	void Pop();
	T Peek();

private:

	T       m_Buffer[N];
	int32   m_nTop = -1;
};

template<typename T, uint32 N>
void MyStack<T, N>::Push(const T& val)
{
	if (m_nTop == N - 1) { return; }

	++m_nTop;
	m_Buffer[m_nTop] = val;
}

template<typename T, uint32 N>
void MyStack<T, N>::Pop()
{
	if (m_nTop < 0) { throw eStackException::EmptyBuffer; }
	--m_nTop;
}

template<typename T, uint32 N>
T MyStack<T, N>::Peek()
{
	if (m_nTop < 0) { throw eStackException::EmptyBuffer; }

	return m_Buffer[m_nTop];
}


enum numberBit
{
	intBit = 32,
};

template<uint32 N>
class MyStack<int, N>
{
public:

	void Push(const int& val)
	{
		{
			if (m_nTop == N - 1) { return; }

			++m_nTop;
			m_Buffer[m_nTop] = val;
		}

	}

	void Pop();
	int Peek();

private:

	int     m_Buffer[N];
	int32   m_nTop = -1;
};

template<uint32 N>
class MyStack<bool, N>
{
public:

	void Push(const bool& val)
	{
		{
			if (m_nTop == N - 1) { return; }

			++m_nTop;
			m_Buffer[m_nTop/intBit] += (val << (m_nTop%intBit));
		}
	}

	void Pop();
	int Peek();

	int     m_Buffer[N / intBit + 1] = { 0, };
	int32   m_nTop = -1;
};

template<uint32 N>
void MyStack<bool, N>::Pop()
{
	if (m_nTop < 0) { throw eStackException::EmptyBuffer; }
	m_Buffer[m_nTop / intBit] &= (~(1 << (m_nTop%intBit)));
	--m_nTop;
}

template<uint32 N>
int  MyStack<bool, N>::Peek()
{
	if (m_nTop < 0) { throw eStackException::EmptyBuffer; }

	return (m_Buffer[m_nTop / intBit] >> (m_nTop%intBit)) & 1;
}
int main() {
	MyStack<bool, intBit*2> a;
	try {
		a.Peek();
	}
	catch (eStackException e) {
		cout << static_cast<int>(e) << endl;
		cout << " warning" << endl;
	}

	srand(time(0));
	for (int i = 0; i < intBit*2; ++i) {		
		a.Push(rand()%2);
		cout << a.Peek() << ' ';
		// a.Push(1);
		cout << bitset<intBit>(a.m_Buffer[a.m_nTop / intBit]) << endl;
	}

	for (int i = 0; i < intBit*2; ++i) {
		cout << a.Peek() << ' ';
		cout << bitset<intBit>(a.m_Buffer[a.m_nTop / intBit]) << endl;
		a.Pop();
	}

	try {
		a.Peek();
	}
	catch (eStackException e) {
		cout << (int)e << endl;
		cout << "wwarning" << endl;
	}
}