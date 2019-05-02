// any_of example
#include "pch.h"
#include <iostream>     // std::cout
#include <vector>
#include <string>
#include <algorithm>


using namespace std;

using int32 = signed int;
using int8 = signed char;

using uint32 = unsigned int;
using uint8 = unsigned char;

template<typename T>
T Add(const T& a, const T& b) {
	return T(a + b);
}

template<>
double Add(const double& a, const double& b) {
	return (a + b);
}

class A {
public:
	A() = default;
	explicit A(const char* _input) {
		if (_input != nullptr) {
			int length = strlen(_input);
			char* buffer = new char[length + 1];
			buffer[length] = 0;
			text = buffer;
		}
	}
	A(A& _input) : A(_input.text) {}
	A(A&& _input) : A(_input.text) {
		delete[] _input.text;
		_input.text = nullptr;
	}
	~A() {
		delete[] text;
		text = nullptr;
	}
	A operator+(const char* _input) {
		if (_input == nullptr) {
			return *this;
		}
		int lengthInput = strlen(_input);
		if (text == nullptr) {
			return A(_input);
		}
		int lengthThis = strlen(text);
		int lengthNew = lengthThis + lengthInput + 1;
		char* temp = new char[lengthNew];
		strcpy_s(temp, lengthThis, text);
		strcat_s(temp, lengthInput, _input);
		A tempA;
		tempA.text = temp;
		return tempA;
	}
	A operator+(const A& _input) {
		return *this + A(_input.text);
	}


	char* text = nullptr;

};

int main() {

	A a1, a2, a3, a4, a5, a6, a7 ,a8 ,a9, a10;
	const char* txt1 = "brown fox";
	const char* txt2 = "lazy dog";
	a3 = a1 + a2;
	a4 = a1 + txt1;
	a5 = a1 + txt2;
	a6 = a4 + a5;
	a7 = Add(a1, a2);
	a10 = Add(a4, a5);

	return 0;
}