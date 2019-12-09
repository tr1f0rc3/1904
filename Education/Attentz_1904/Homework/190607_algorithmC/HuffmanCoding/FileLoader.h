#pragma once
#include <vector>
#include <string>

class FileLoader
{
public:
	std::vector<std::string> loadTxt(const std::string& path) const;
	bool saveTxt(const std::vector<std::string>& text, const std::string& path) const;

};

