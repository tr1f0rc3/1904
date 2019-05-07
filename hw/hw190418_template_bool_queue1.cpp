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

template <typename T, uint32 N>
class myqueue {
public:

	bool pop();
	T front() const;
	bool push(const T& _input);

	T buffer[N]{ 0, };
	int32 currRear = -1;
	int32 currFront = -1;
};


template <typename T, uint32 N>
inline bool myqueue<T, N>::pop() {
	if (currFront < currRear) {
		buffer[++currFront] = 0;
		return 0;
	}
	else {
		cout << "empty" << endl;
		return -1;
	}
}

template <typename T, uint32 N>
inline T myqueue<T, N>::front() const {
	if (currFront < currRear) {
		return T(buffer[currFront + 1]);
	}
	else {
		cout << "empty" << endl;
		return T();
	}
}

template <typename T, uint32 N>
inline bool myqueue<T, N>::push(const T& _input) {
	if (currRear < static_cast<int>(N - 1)) {
		buffer[++currRear] = _input;
	}
	else if (currFront > 0) {
		for (int i = 0; i < currRear - currFront; ++i) {
			buffer[i] = buffer[currFront + i + 1];
			buffer[currFront + i + 1] = 0;
		}
		currRear = currRear - currFront - 1;
		currFront = -1;
	}
	else {
		cout << "full" << endl;
		return -1;
	}
	return 0;
}



template <uint32 N>
class myqueue<bool, N> {
public:
	static const uint32 bitInt = sizeof(int) * 8;

	bool pop();
	bool front() const;
	bool push(const bool& _input);

	int buffer[N / bitInt + 1]{ 0, };
	int32 currRear = -1;
	int32 currFront = -1;
	
};



template <uint32 N>
inline bool myqueue<bool, N>::pop() {
	if (currFront < currRear) {
		++currFront;
		buffer[currFront / bitInt] &= ~(1 << ((currFront) % bitInt));
		return 0;
	}
	else {
		cout << "empty" << endl;
		return -1;
	}
}

template <uint32 N>
inline bool myqueue<bool, N>::front() const {
	if (currFront < currRear) {
		uint32 toPrint = currFront + 1;
		return (1 & (buffer[toPrint / bitInt] >> (toPrint % bitInt)));
	}
	else {
		cout << "empty" << endl;
		return -1;
	}
}

template <uint32 N>
inline bool myqueue<bool, N>::push(const bool& _input) {
	if (currRear < static_cast<int>(N - 1)) {
		++currRear;
		buffer[(currRear)/bitInt] += (_input << (currRear % bitInt));
	}
	else if (currFront > 0) {
		uint32 PosOfToMove, OldBuffMovedElem, BitMovedElem, BitsToMove, NewBuffMovedElem;
		for (int i = 0; i < currRear - currFront; ++i) {
			PosOfToMove = currFront + i + 1;
			OldBuffMovedElem = PosOfToMove / bitInt;
			BitMovedElem = PosOfToMove % bitInt;
			BitsToMove = 1 & (buffer[OldBuffMovedElem] >> BitMovedElem);
			NewBuffMovedElem = i / bitInt;
			buffer[NewBuffMovedElem] += BitsToMove << i ;
			buffer[OldBuffMovedElem] &= ~(1 << BitMovedElem);
		}
		currRear = currRear - currFront - 1;
		currFront = -1;
		push(_input);
	}
	else {
		cout << "full" << endl;
		return -1;
	}
	return 0;
}


int main() {
	enum { size1 = 3, intBit = 32, size2 = 40 };

	{
		myqueue<bool, size2> a;

		srand(time(0));
		for (int i = 0; i < size2 + 2; ++i) {
			a.push(1);
			//a.push(rand() % 2);
			cout << a.front() << ' ';
			cout << bitset<intBit>(a.buffer[1]) << bitset<intBit>(a.buffer[0])<< endl;
		}

		for (int i = 0; i < size2-2; ++i) {
			a.pop();
			cout << a.front() << ' '; 
			cout << bitset<intBit>(a.buffer[1]) << bitset<intBit>(a.buffer[0]) << endl;

		}

		for (int i = 0; i < 32; ++i) {
			a.push(1);
			//a.push(rand() % 2);
			cout << a.front() << ' ';
			cout << bitset<intBit>(a.buffer[1]) << bitset<intBit>(a.buffer[0]) << endl;

		}
		cout << endl;
	}

	{
		myqueue<int, size1> a;

		for (int i = 0; i < size1 + 1; ++i) {
			a.push(i);
			cout << a.front() << ' ';
			for (int j = 0; j < size1; ++j) {
				cout << a.buffer[j];
			}
			cout << endl;
		}

		for (int i = 0; i < size1 + 1; ++i) {
			a.pop();
			cout << a.front() << ' ';
			for (int j = 0; j < size1; ++j) {
				cout << a.buffer[j];
			}
			cout << endl;
		}
		cout << endl;
	}

}
