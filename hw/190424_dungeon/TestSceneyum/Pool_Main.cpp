#include "pch.h"

#include "ObjectPool.h"


int main()
{
	using namespace std;

	ObjectPool<int> pool(100);
	std::vector<int*> vcInt;
	for (int i = 0; i < 105; ++i)
	{
		vcInt.push_back(pool.Get());
	}

	cout << pool.GetFreeObjectCount() << endl;
	cout << pool.GetMaxFreeObjectCount() << endl;

	for (auto pVal : vcInt)
	{
		pool.Return(pVal);
	}

	vcInt.clear();

	cout << pool.GetFreeObjectCount() << endl;
	cout << pool.GetMaxFreeObjectCount() << endl;


	for (auto pVal : vcInt)
	{
		delete pVal;
	}
}
