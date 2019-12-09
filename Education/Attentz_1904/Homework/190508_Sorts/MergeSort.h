#pragma once

template <typename T>
void mergeSort(const typename std::vector<T>::iterator& begIt, const typename std::vector<T>::iterator& endIt) {
	size_t vecSize = endIt - begIt;
	if (vecSize == 1) {
		return;
	}
	auto midIt = begIt + vecSize / 2 + vecSize % 2;
	if (vecSize > 2) {
		mergeSort<T>(begIt, midIt);
		mergeSort<T>(midIt, endIt);
	}
	for (auto it = begIt; it != endIt;) {
		std::cout << *it++;
	}
	std::cout << ' ' << vecSize << ' ' << *begIt << ' ' << *(endIt-1) << std::endl;
	std::vector<T> tempVec(vecSize);
	auto tempIt = tempVec.begin();
	auto leftIt = begIt;
	auto rightIt = midIt;
	while (leftIt != midIt && rightIt != endIt) {
		if (*leftIt <= *rightIt) {
			*tempIt++ = *leftIt++;
		}
		else if (*leftIt >= *rightIt) {
			*tempIt++ = *rightIt++;
		}
	}
	while (leftIt != midIt) {
		*tempIt++ = *leftIt++;
	}
	while (rightIt != endIt) {
		*tempIt++ = *rightIt++;
	}
	assert(tempIt == tempVec.end());
	tempIt = tempVec.begin();
	for (auto it = begIt; it != endIt;) {
		*it++ = *tempIt++;
	}
	for (auto it = begIt; it != endIt;) {
		std::cout << *it++;
	}
	std::cout << ' ' << vecSize << ' ' << *begIt << ' ' << *(endIt-1) << std::endl;
}
