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

template <typename T>
void insertionSort02(std::vector<T> *vec) {
	for (size_t i = 1; i != vec->size(); ++i) {
		T temp = *(vec->begin() + i);
		size_t j = i;
		for (; j != 0; --j) {
			if ((*vec)[j - 1] > temp) {
				(*vec)[j] = (*vec)[j - 1];
			}
			else {
				break;
			}
		}
		(*vec)[j] = temp;
	}
}