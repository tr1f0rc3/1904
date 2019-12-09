#pragma once
class RunLen
{
	enum {
		compressedWordLength = 3,
	};

public:
	RunLen();
	~RunLen();

	void loadFile(std::string _file = "abc.txt");
	std::string compress(const std::string& input) const;
	std::string extract(const std::string& input) const;

protected:
	std::vector<std::string> data;

};

