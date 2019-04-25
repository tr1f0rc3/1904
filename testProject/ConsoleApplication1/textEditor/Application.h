#pragma once

#include "Editor.h"
#include "Command.h"

class Command;

class Application
{
public:

	bool addEditor(Editor* _editor);
	bool setEditor(int n);
	bool copy(int _pos, int _length);
	bool cut(int _pos, int _length);
	bool replace(int _pos, int _length);
	bool paste(int _pos);
	bool undo();

	std::string clipBoard;
	Editor* activeEditor;
	std::vector<Editor*> editors;
	std::vector<Command*> commandHistory;
};

