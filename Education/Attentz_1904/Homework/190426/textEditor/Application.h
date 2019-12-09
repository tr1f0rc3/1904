#pragma once

#include "Editor.h"
#include "Commands.h"

class Command;

class Application
{
public:
	Application() = default;
	virtual ~Application();

	bool addEditor(Editor* _editor);
	bool setEditor(int n);

	bool executeCmd(Command* _cmd);

	bool copy(int _pos, int _length);
	bool cut(int _pos, int _length);
	bool replace(int _pos, int _length);
	bool paste(int _pos);
	bool undo();

	std::string clipBoard = "";
	Editor* activeEditor = nullptr;
	std::vector<Editor*> editors;
	MacroCommand cmdHistory;
};

