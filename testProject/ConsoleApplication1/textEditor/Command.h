#pragma once

#include "Editor.h"

class Application;

class Command {
public:
	Command(std::string* _appText, Editor* _edit, int _pos = 0, int _length = 0, const std::string& _text = "");
	virtual ~Command() {}

	bool saveBackUp();
	bool undo();
	virtual bool execute() = 0;

protected:
	std::string text = "";
	std::string backUp = "";	
	std::string* appText;
	Editor* editor;

	int pos;
	int length;
};

struct MacroCommand {
	virtual ~MacroCommand();

	bool push(Command* _cmd);
	Command* pop();

	std::stack<Command*> cmdStack;

};