#pragma once

#include "Command.h"



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
		int pos, int length, const std::string& _text);
	virtual ~ReplaceCommand() {}
	virtual bool execute() override;

};

class PasteCommand : public Command {
public:
	PasteCommand(std::string* _appText, Editor* _edit,
		int pos, const std::string& _text);
	virtual ~PasteCommand() {}
	virtual bool execute() override;

};

class UndoCommand : public Command {
public:
	UndoCommand(std::string* _appText, Editor* _edit, MacroCommand* _macro);
	virtual ~UndoCommand() {}
	virtual bool execute() override;

	MacroCommand* macroCmd;

};
