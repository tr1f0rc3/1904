
std::vector<std::string> readText(std::string _filePath) {
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

void writeText(std::string _filePath, std::vector<std::string> _data) {
	std::ofstream writeFile(_filePath.data());
	assert(writeFile.is_open());
	assert(_data.size() > 0);
	for (auto& elem : _data) {
		writeFile << elem << std::endl;
	}
	writeFile.close();
}

std::vector<std::string> lineToWords(std::string _data) {
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

std::tuple<std::string, std::string, int> wordsToTuple(std::vector<std::string> _data) {
	assert(_data.size() == 3);
	std::string word1 = static_cast<std::string>(_data[0]);
	std::string word2 = static_cast<std::string>(_data[1]);
	int number1 = stoi(_data[2]);
	return std::make_tuple(word1, word2, number1);
	// enum class to int is possible
	// enum class to string is impossible string '0' to enum is possible;
}

std::vector<std::string>& makeMapRectangle(std::vector<std::string>&& _data) {
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
