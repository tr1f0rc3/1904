1. 987654321
     87654321
      7654321
       654321
         54321
          4321
            321
              21
                1

-----------------------

2. 구구단 출력

---------------------------

3. void 십진수숫자_to_해당진수문자열 함수제작.

10진수의 숫자를 넣으면. 2인자로 넣은 숫자의 ( 2~16까지 )

진수와 해당 숫자를 표기.

ex) 10, 2 ----    2진수 : 1010
     10, 8 ----    8진수 : 12 

 // 못하겠거든 10진수를 받으면 2진수로만이라도 표현해보세요

4. strlen, strcpy, bool strsame 구현

5. 

도형 클래스를 상속받아서
정사각형 / 직사각형 / 이등변삼각형 제작.

도형 클래스의 경우 left, top, right, bottom 값을 가지고있고,
3개 상속받은 클래스는 추가적인 값이 필요없음.

virtual const char* Name() = 0;
virtual size_t Size() = 0;

10~15개 사이로 랜덤하게 버퍼를 생성. ( 도형 클래스 배열 )
랜덤하게 정사각형 / 직사각형 / 이등변 삼각형 넣고,
크기의 경우 1~i 정도로 랜덤하게 각각 수치를 넣습니다.

각각 Name(), Size() 호출. ( 값은 잘 나와야겠죠 )


6.

struct Rect { left, top, right, bottom };

int GetCrossSize( const Rect& a, const Rect& b )

두 사각형의 위치를 받아서 서로 겹치는 크기를 리턴하는 함수제작.

두 사각형은 left, top, right, bottom 4개인자의


#include <iostream>
#include <vector>
#include <iomanip>
#include <string>
#include <algorithm>
#include <cmath>
#include <cstddef>
#include <array>
#include <ctime>
#include <random>

using namespace std;

#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE

// problem 1, 2, 3
void problem1() {
	enum {cnt = 9,};
	for (int i = cnt; i != 0; --i) {
		cout << string(cnt - i, ' ');
		for (int j = i; j != 0; --j) {
			cout << j;
		}
		cout << endl;
	}
}

void problem2() {
	for (int i = 1; i != 10; ++i) {
		for (int j = 1; j != 10; ++j) {
			cout << i << '*' << j << '=' << i * j << ' ';
		}
		cout << endl;
	}
}

int problem3(int input, int jinsu) {

	if (!(input >= 1 && jinsu >= 2)) {
		cout << "input error" << endl;
		return -1;
	}

	int dividend(input), powerCnt(0);
	while (dividend > jinsu) {
		dividend /= jinsu;
		++powerCnt;
	}
	vector<int> vec;
	int subtracted = input;
	while (powerCnt != 0) {
		int jinsuPower(1), constant(0);
		for (int i = powerCnt; i != 0; --i) {
			jinsuPower *= jinsu;
		}
		int hightoLowNumber = subtracted / jinsuPower;
		vec.push_back(hightoLowNumber);
		subtracted -= (hightoLowNumber * jinsuPower);
		--powerCnt;
	}
	vec.push_back(subtracted);
	cout << jinsu << "jinsu: ";
	for (auto& elem : vec) {
		cout << elem;
	}
	cout << endl;

	return 0;
}

// problem 4
int mystrlen(const char* _input) {
	if (_input == nullptr)
		return 0;
	else {
		int cnt(0);
		while (*_input++) {
			++cnt;
		}
		return cnt;
	}
}

char* mystrcpy(char* _receive, int _numbers, const char* _give) {
	if (_give == nullptr || _numbers == 0) {
		cout << "giver is a nullptr" << endl;
		return _receive;
	}
	/*int lenOld(0);
	if (_receive != nullptr) {
		lenOld = mystrlen(_receive);
		delete[] _receive;
	}
	if (lenOld < _numbers) {*/

	char* temp = new char[_numbers + 1];
	for (int i = 0; i < _numbers; ++i) {
		temp[i] = _give[i];
	}
	temp[_numbers] = 0;
	_receive = temp;
	return _receive;
}

bool mystrsame(const char* lhs, const char* rhs) {
	if (lhs == nullptr || rhs == nullptr) {
		if (lhs == nullptr && rhs == nullptr) {
			return true;
		}
		return false;
	}
	int len = strlen(lhs);
	if (len != strlen(rhs)) {
		return false;
	}
	for (int i = 0; i != len; ++i) {
		if (lhs[i] != rhs[i]) {
			return false;
		}
	}
	return true;
}

// problem 5
// dohyung
class dohyung {

public:
	dohyung() = default;
	dohyung(int _left, int _top, int _right, int _bottom);
	virtual ~dohyung() {}

	virtual const char* Name() const = 0;
	virtual size_t Size() const = 0;

protected:
	int left = 0;
	int top = 0;
	int right = 0;
	int bottom = 0;

	int width = 0;
	int height = 0;
};

inline dohyung::dohyung(int _left, int _top, int _right, int _bottom) {
	if (_right < _left) {
		left = _right;
		right = _left;
	}
	if (_top < _bottom) {
		top = _bottom;
		bottom = _top;
	}
	width = _right - _left;
	height = _top - _bottom;
}

// retangle
class rectangle : public dohyung {
	rectangle(int _left, int _top, int _right, int _bottom) : dohyung(_left, _top, _right, _bottom) {}

	virtual const char* Name() const override;
	virtual size_t Size() const override;
};

inline const char* rectangle::Name() const {
	return "rectangle";
}

inline size_t rectangle::Size() const {
	size_t size = width * height;
	return size;
};

class square : public rectangle {
	square(int _left, int _top, int _right, int _bottom) : rectangle(_left, _top, _right, _bottom) {}

	virtual const char* Name() const override;
};

// square
inline const char* square::Name() const {
	return "square";
}

// triangle
class triangle : public dohyung {
	triangle(int _left, int _top, int _right, int _bottom) : dohyung(_left, _top, _right, _bottom) {}

	virtual const char* Name() const override;
	virtual size_t Size() const override;
};

inline const char* triangle::Name() const {
	return "triangle";
}

inline size_t triangle::Size() const {
	size_t size = width * height / 2;
	return size;
}

vector<dohyung>* dohyungVec() {
	srand(time(0));
	int sizeRandom = 10 + (rand() % 6);
	vector<dohyung>* dohyungContainer = new vector<dohyung>;
	enum { lines = 4, };
	vector<int> lengthVec(lines);

	for (int i = 0; i != sizeRandom; ++i) {
		int dohyungRandom = rand() / 3;
		for (int j = 0; j == lines; ++j) {
			lengthVec[j] = 1 + (rand() % 5);
		}
		switch (dohyungRandom) {
		case 0: {
			//square tempDohyung(lengthVec[0], lengthVec[1], lengthVec[2], lengthVec[3]);
			//dohyungContainer.pushback(tempDohyung);
			break;
		}
		//case 1: {
		//	dohyungContainer.pushback(rectangle(lengthVec[0], lengthVec[1], lengthVec[2], lengthVec[3]));
		//	break;
		//}
		//case 2: {
		//	dohyungContainer.pushback(triangle(lengthVec[0], lengthVec[1], lengthVec[2], lengthVec[3]));
		//	break;
		//}
		defualt:
			break;
		}

	}
	return dohyungContainer;
}

int main() {

	// problem 1 & 2
	{
		problem1();
		cout << endl;
		problem2();
		cout << endl;
	}

	// problem 3
	{
		cout << "problem 3" << endl;
		problem3(87, 2);
		problem3(107, 8);
		// except
		problem3(10, 1);
		problem3(0, 2);
		cout << endl;
	}

	// problem 4-1
	{
		cout << "problem 4-1" << endl;
		// null
		const char* nullInput = nullptr;
		int a = mystrlen(nullInput);
		cout << "strlen: " << a << endl;
		// 
		const char* brownInput = "brown fox";
		a = mystrlen(brownInput);
		cout << "strlen: " << a << endl;
		cout << endl;


	}

	// problem 4-2
	{
		const char* _give("brown fox");
		char _receive1[10] = "lazy dog";
		char* a = mystrcpy(_receive1, 5, _give);
		cout << "cpy: " << a << endl;
		delete[] a; a = nullptr;

		a = mystrcpy(_receive1, 10, _give);
		cout << "cpy: " << a << endl;
		delete[] a; a = nullptr;

		char _receive2[5] = "lazy";
		a = mystrcpy(_receive2, 10, _give);
		cout << "cpy: " << a << endl;
		delete[] a; a = nullptr;

		a = mystrcpy(_receive2, 10, nullptr);
		cout << "cpy: " << a << endl;
		cout << endl;
	}

	// problem 4-3
	{
		const char* str1("brown fox");
		const char* str2("brown box");
		const char* str3("lay dog");
		cout << mystrsame(nullptr, nullptr) << ' ';
		cout << mystrsame(nullptr, str1) << ' ';
		cout << mystrsame(str1, nullptr) << ' ';
		cout << mystrsame(str1, str1) << ' ';
		cout << mystrsame(str1, str2) << ' ';
		cout << mystrsame(str1, str3) << ' ';
	}

	// problem 5
	{
		vector<dohyung>* vec = dohyungVec();
		triangle teeemp(1, 2, 3, 4);
		for (auto& elem : *vec) {
			elem.Name();
			elem.Size();
		}
	}






	getchar();


}