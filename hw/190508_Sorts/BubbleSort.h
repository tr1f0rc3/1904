#pragma once

template <typename T>
void bubbleSort(std::vector<T> *vec) {
	bool bChange = false;
	for (size_t i = 0; i != vec->size() - 1; ++i) {
		for (auto it = vec->begin() + 1; it != vec->end() - i; ++it) {
			if (*(it - 1) > *it) {
				std::swap(*(it - 1), *it);
				bChange = true;
			}
		}
		if (bChange == false) break;
	}
}