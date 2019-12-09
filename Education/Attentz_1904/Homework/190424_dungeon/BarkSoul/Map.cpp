#include "pch.h"
#include "Map.h"


std::vector<std::string> Map::readText(std::string _filePath) {
	std::vector<std::string> vec;
	std::ifstream openFile(_filePath.data());
	assert(openFile.is_open());
	std::string line;
	while (std::getline(openFile, line)) {
		vec.push_back(line);
	}
	openFile.close();
	return vec;
};

void Map::writeText(std::string _filePath, std::vector<std::string> _data) {
	std::ofstream writeFile(_filePath.data());
	assert(writeFile.is_open());
	assert(_data.size() > 0);
	for (auto& elem : _data) {
		writeFile << elem << std::endl;
	}
	writeFile.close();
}

std::vector<std::string> Map::lineToWords(std::string _data) {
	assert(_data.size() > 0);
	std::string word;
	std::vector<std::string> vec;
	std::istringstream iss(_data);
	do {
		iss >> word;
		vec.push_back(word);
	} while (iss);
	return vec;
}

std::vector<std::string>& Map::makeMapRectangle(std::vector<std::string>& _data) {
	size_t currLength, maxLength(0);
	std::string newLine;
	for (auto it = _data.begin(); it != _data.end(); ++it) {
		if (currLength = it->length()) maxLength = currLength;
		else {
			newLine = *it + std::string(maxLength - currLength, '#');
			*it = newLine;
		}
	}
	return _data;
}

enum { mapLimit = 200 };

void Map::loadMap(std::string _filePath, char (&_input)[mapLimit][mapLimit] ) {
	std::vector<std::string> vec = readText(_filePath);
	vec = makeMapRectangle(vec);
	int i(0), j(0);
	for (auto& str : vec) {
		for (auto& cha : str) {
			_input[i][j++] = cha;
		}
		j = 0; ++i;
	}
}

