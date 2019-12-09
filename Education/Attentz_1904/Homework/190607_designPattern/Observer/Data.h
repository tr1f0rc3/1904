#pragma once
#include "pch.h"

class Data {
public:
	Data();
	std::vector<std::string> getData(int escene) const;

protected:
	std::map<int, std::vector<std::string>> data;

};