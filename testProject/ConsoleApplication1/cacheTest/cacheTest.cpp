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


#define TestNum(nAr, nCount) \
Test<nAr>* p##nAr = new Test<nAr>[nCount]; \
tStamp = timeGetTime(); \
for (int i = 0; i < nCount; ++i) \
{ \
    SetValue<nAr>(&p##nAr[nAr]); \
} \
now = timeGetTime() - tStamp; \
cout << nAr << " = 걸린시간 : " << now << endl;

#define DeleteTest(nAr) \
delete p##nAr;\
p##nAr = nullptr;


int main()
{
	using namespace std;

	constexpr int nCount = 250000;
	DWORD tStamp = 0;
	DWORD now = 0;

	TestNum(1, nCount);
	TestNum(2, nCount);
	TestNum(16, nCount);
	TestNum(128, nCount);
	TestNum(512, nCount);

	DeleteTest(1);
	DeleteTest(2);
	DeleteTest(16);
	DeleteTest(128);
	DeleteTest(512);




	return 0;
}
