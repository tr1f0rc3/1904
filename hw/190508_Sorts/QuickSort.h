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