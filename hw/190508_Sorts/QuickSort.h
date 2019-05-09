#pragma once

template <typename T>
void quickSort(const typename std::vector<T>::iterator& begIt, const typename std::vector<T>::iterator& endIt) {
	if (endIt - begIt == 1) {
		return;
	}
	T key = *(endIt - 1);
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
		++itl;
		if (begIt != itr) {
			--itr;
		}
	}
	quickSort<T>(begIt, itl);
	quickSort<T>(itl + 1, endIt);

}