#pragma once

#include "Editor.h"

class Application;

class Command
{
public:
	Command(std::string* _appText, Editor* _edit, int _pos = 0, int _length = 0);
	virtual ~Command() {}

	bool saveBackUp();
	bool undo();
	virtual bool execute() = 0;

protected:
	std::string text = "";
	std::string* appText;
	Editor* editor;

	int pos;
	int length;
};


class CopyCommand : public Command {
public:
	CopyCommand(std::string* _appText, Editor* _edit,
		int pos, int length);
	virtual ~CopyCommand() {}
	virtual bool execute() override;

};

class CutCommand : public Command {
public:
	CutCommand(std::string* _appText, Editor* _edit,
		int pos, int length);	
	virtual ~CutCommand() {}
	virtual bool execute() override;

};

class ReplaceCommand : public Command {
public:
	ReplaceCommand(std::string* _appText, Editor* _edit,
		int pos, int length);
	virtual ~ReplaceCommand() {}
	virtual bool execute() override;

};

class PasteCommand : public Command {
public:
	PasteCommand(std::string* _appText, Editor* _edit,
		int pos, int length);
	virtual ~PasteCommand() {}
	virtual bool execute() override;

};

