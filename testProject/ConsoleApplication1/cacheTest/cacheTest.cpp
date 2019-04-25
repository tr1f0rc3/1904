#include "pch.h"
#include <vld.h>
#include <iostream>
#include <windows.h>

#pragma comment(lib, "winmm.lib")

template<size_t n>
class Test
{
public:
	int ar[n];
};

template<size_t n>
void SetValue(Test<n>* t)
{
	t->ar[0] = 10;
}



int main()
{
	using namespace std;

	constexpr int nCount = 250000;

	Test<1>* p1 = new Test<1>[nCount];

	auto tStamp = timeGetTime();

	for (int i = 0; i < nCount; ++i)
	{
		SetValue<1>(&p1[i]);
	}

	auto now = timeGetTime() - tStamp;

	cout << 1 << " = 걸린시간 : " << now << endl;



	Test<16>* p16 = new Test<16>[nCount];

	tStamp = timeGetTime();

	for (int i = 0; i < nCount; ++i)
	{
		SetValue<16>(&p16[i]);
	}

	now = timeGetTime() - tStamp;

	cout << 16 << " = 걸린시간 : " << now << endl;



	Test<64>* p64 = new Test<64>[nCount];

	tStamp = timeGetTime();

	for (int i = 0; i < nCount; ++i)
	{
		SetValue<64>(&p64[i]);
	}

	now = timeGetTime() - tStamp;

	cout << 64 << " = 걸린시간 : " << now << endl;


	Test<256>* p256 = new Test<256>[nCount];

	tStamp = timeGetTime();

	for (int i = 0; i < nCount; ++i)
	{
		SetValue<256>(&p256[i]);
	}

	now = timeGetTime() - tStamp;

	cout << 256 << " = 걸린시간 : " << now << endl;


	Test<1024>* p1024 = new Test<1024>[nCount];

	tStamp = timeGetTime();

	for (int i = 0; i < nCount; ++i)
	{
		SetValue<1024>(&p1024[i]);
	}

	now = timeGetTime() - tStamp;

	cout << 1024 << " = 걸린시간 : " << now << endl;



	delete[] p1;
	delete[] p16;
	delete[] p64;
	delete[] p256;
	delete[] p1024;


	return 0;
}
