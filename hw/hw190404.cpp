#include "pch.h"
#include <iostream>

// 열거 멤버의 이름은 컴파일 중에만 사용되는 임시 값이다. 컴파일 후에는 정수 상수와 같아진다.

using namespace std;
int main()
{
	{

		std::cout << "Hello World!\n";

	}

	{

		int i, j;
		for (i = 1;i <= 15;i = i + 1) {
			for (j = 0;j < i;j = j + 1) {
				printf("*");
			}
			printf("\n");
		}

	}

	{

		short a, b, c;
		unsigned short s, t, u;

		a = 20000;
		b = 30000;
		c = a + b;
		printf("%d+%d=%d\n", a, b, c);

		s = 20000;
		t = 30000;
		u = s - t;
		printf("%d-%d=%d\n", s, t, u);

	}
	{

		__int64 a, b, c;

		a = 111111111;
		b = 111111111;
		c = a * b;
		printf("%I64d\n", c);

	}

	{

		double d;
		d = 123.456;

		printf("고정 소수점 : %f\n", d);
		printf("부동 소수점 : %e\n", d);
		printf("일반형 : %g\n", d);

	}

	{

		char ch;
		ch = 'A';
		cout << ch << endl;

	}

	{
		enum fruit { apple, orange, banana };
		fruit a;
		a = orange;
		cout << a << endl;
	}
}
