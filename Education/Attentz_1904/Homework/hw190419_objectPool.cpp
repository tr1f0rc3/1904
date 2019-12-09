#include "pool.h"

#include <vector>
#include <iostream>



int main()
{
	using namespace std;

	Pool<int> pool(100);
	std::vector<int*> vcInt;
	for (int i = 0; i < 105; ++i)
	{
		vcInt.push_back(pool.get());
	}

	cout << pool.getFreeObjectCnt() << endl;
	cout << pool.getMaxFreeObjectCnt() << endl;

	for (auto pVal : vcInt)
	{
		pool.leave(pVal);
	}

	vcInt.clear();

	cout << pool.getFreeObjectCnt() << endl;
	cout << pool.getMaxFreeObjectCnt() << endl;


	for (auto pVal : vcInt)
	{
		delete pVal;
	}
}

