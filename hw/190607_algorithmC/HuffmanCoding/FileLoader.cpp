#include "FileLoader.h"
#include <fstream>

std::vector<std::string> FileLoader::loadTxt(const std::string& path) const {
	std::ifstream ifs(path);
	std::string temp;
	std::vector<std::string> toReturn;
	while (std::getline(ifs, temp)) {
		toReturn.emplace_back(temp);
	}
	return toReturn;
}


bool FileLoader::saveTxt(const std::vector<std::string>& text, const std::string& path) const {
	std::ofstream file(path);
	for (auto& itr : text) {
		file << itr << std::endl;
	}
	return true;
}
