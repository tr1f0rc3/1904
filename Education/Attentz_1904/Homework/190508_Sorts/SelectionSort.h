#pragma once

template <typename T>
void selectionSort(std::vector<T> *vec) {
	for (auto it1 = vec->begin(); it1 != vec->end() - 1; ++ it1) {
		auto it2 = std::min_element(it1 + 1, vec->end());
		if (*it2 < *it1) {
			std::swap(*it1, *it2);
		}
	}
}