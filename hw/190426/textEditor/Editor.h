#pragma once
class Editor
{
public:
	
	virtual ~Editor() {}
	std::string getText(int _pos, int _length) const;
	bool deleteText(int _pos, int _length);
	bool replaceText(int _pos, int _length, const std::string& _text);
	bool insertText(int _pos, const std::string& _text);


	std::string text = std::string("");

};

