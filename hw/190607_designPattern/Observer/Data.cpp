#include "pch.h"
#include "Data.h"

Data::Data() {
	data[0] = {};
	data[1] = {"Title", "Option", "Quit"};
	data[2] = { "Loading"};
	data[3] = {"Game", "Restart"};
}

std::vector<std::string> Data::getData(int escene) const {
	auto it = data.find(escene);
	if (it == data.end()) {
		return std::vector<std::string>();
	}
	else {
		return it->second;
	}
}

