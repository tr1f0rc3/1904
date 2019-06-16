#pragma once
#include <vector>
#include <string>

class FileLoader
{
public:
	std::vector<std::string> operator()(std::string path = "song.txt");
};

