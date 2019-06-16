#include "FileLoader.h"
#include <fstream>

std::vector<std::string> FileLoader::operator()(std::string path) {
	std::ifstream ifs(path);
	std::string temp;
	std::vector<std::string> toReturn;
	while (std::getline(ifs, temp)) {
		toReturn.emplace_back(temp);
	}
	return toReturn;
}
