#include "pch.h"
#include "RunLen.h"


RunLen::RunLen()
{
}


RunLen::~RunLen()
{
}

void RunLen::loadFile(std::string _file) {
		//std::ifstream file(_file);

		//char buffer[1024]{ 0, };

		//while (!file.eof())
		//{
		//	file.getline(buffer, 1024);

		//	cout << buffer << endl;
		//}

		//file.close();
}

std::string RunLen::compress(const std::string& input) const {
	std::string output;
	if (input.length() > 0) {
		char oldChar, newChar;
		int cnt(0);
		oldChar = input[0];
		for (size_t i = 0; i <= input.length(); ++i) {
			if (i < input.length()) {
				newChar = input[i];
				if (newChar == oldChar) {
					++cnt;
				}
			}
			if (newChar != oldChar || i == input.length()) {
				if (cnt > compressedWordLength) {
					output += '^';
					output += oldChar;
					output += (char)(cnt);
				}
				else {
					output += std::string(cnt, oldChar);
				}
				oldChar = newChar;
				cnt = 1;
			}
		}
	}
	else {
		output = "";
	}
	return output;
}


std::string RunLen::extract(const std::string& input) const {
	std::string output;
	if (input.length() > 0) {
		char currChar;
		for (size_t i = 0; i < input.length(); ++i) {
			currChar = input[i];
			if (currChar == '^') {
				output += std::string(static_cast<int>(input[i + 2]), input[i + 1]);
				i += (compressedWordLength - 1);
			}
			else {
				output += currChar;
			}
		}
	}
	else {
		output = "";
	}
	return output;
}
