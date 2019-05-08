#pragma once

template <typename T>
void insertionSort(std::vector<T> *vec) {
	for (size_t i = 1; i != vec->size(); ++i) {
		auto it = vec->begin() + i;
		T temp = *it;
		for (; it != vec->begin(); --it) {
			if (*(it - 1) > temp) {
				*it = *(it - 1);
			}
			else {
				break;
			}
		}
		*it = temp;
	}
}