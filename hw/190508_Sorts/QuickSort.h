#pragma once

template <typename T>
void quickSort(const typename std::vector<T>::iterator& begIt, const typename std::vector<T>::iterator& endIt) {
	if (endIt == begIt || endIt - 1 == begIt) {
		return;
	}
	T& key = *(endIt - 1);
	auto itl = begIt;
	auto itr = endIt - 2;
	while (1) {
		while (*itl < key) {
			++itl;
		}
		while (begIt != itr && *itr > key) {
			--itr;
		}
		if (itr - itl <= 0) {
			break;
		}
		std::swap(*itl, *itr);
		for (auto it = begIt; it != endIt; ++it) {
			std::cout << *it << ' ';
		}
		std::cout << ':' << ' ' << *itl << ' ' << *itr << std::endl;
		++itl;
		if (begIt != itr) {
			--itr;
		}
	}
	std::swap(*itl, key);
	for (auto it = begIt; it != endIt; ++it) {
		std::cout << *it << ' ';
	}
	std::cout << ':' << ' ' << *itl << ' ' << *itr << std::endl;
	quickSort<T>(begIt, itl);
	quickSort<T>(itl + 1, endIt);

}

using int32 = int;

void QuickSort02(std::vector<int>* Arr, int32 Beg, int32 End)
{
	if (End - Beg <= 1)
	{
		return;
	}
	int32 Key = (*Arr)[End - 1];

	int32 Right = End - 2;
	int32 Left = Beg;
	for (;; ++Left, --Right)
	{
		while ((*Arr)[Left] < Key)
		{
			++Left;
		}
		while ((*Arr)[Right] > Key && Right > 0)
		{
			--Right;
		}
		if (Left >= Right)
		{
			break;
		}
		std::swap((*Arr)[Left], (*Arr)[Right]);
		for (auto elem : *Arr) {
			std::cout << elem;
		}
		std::cout << ' ' << Key << ' ' << Left << ' ' << Right << std::endl;
		

	}
	std::swap((*Arr)[Left], (*Arr)[End - 1]);

	for (auto elem : *Arr) {
		std::cout << elem;
	}
	std::cout << ' ' << Key << ' ' << Left << ' ' << Right << std::endl;

	QuickSort02(Arr, Beg, Left);
	QuickSort02(Arr, Left + 1, End);
}

