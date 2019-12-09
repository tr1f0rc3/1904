#pragma once

int GCD(int lhs, int rhs)
{
	if (lhs < rhs)
	{
		std::swap(lhs, rhs);
	}
	if (lhs % rhs == 0)
	{
		return rhs;
	}
	else
	{
		return GCD(lhs - rhs, rhs);
	}
}

struct PP {
	using int32 = int;
	void PrintPrimePow()
	{
		for (int32 i = 3; i != 30; ++i)
		{
			auto It = PrimePow.find(i);
			if (It != PrimePow.end())
			{
				std::cout << i << ' ' << i * i;
			}
			else
			{
				for (auto elem : PrimePow)
				{
					if (GCD(i, elem.first) == 1)
					{
						PrimePow.insert({ i, i * i });
						std::cout << i << ' ' << i * i;
						break;

					}
				}
			}
		}
	}

	std::map<int, int> PrimePow{ {2,4} };
};
