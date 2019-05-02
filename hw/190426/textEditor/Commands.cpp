#include "pch.h"
#include "Commands.h"



//
CopyCommand::CopyCommand(std::string* _appText, Editor* _edit,
	int _pos, int _length)
	: Command(_appText, _edit, _pos, _length) {}

bool CopyCommand::execute() {
	std::string temp = editor->getText(pos, length);
	if (temp == "") return false;
	else {
		*appText = temp;
		return true;
	}
}

//
CutCommand::CutCommand(std::string* _appText, Editor* _edit,
	int _pos, int _length)
	: Command(_appText, _edit, _pos, _length) {}


bool CutCommand::execute() {
	*appText = editor->getText(pos, length);
	if (*appText == "") return false;
	else {
		saveBackUp();
		return editor->deleteText(pos, length);
	}
}

//
ReplaceCommand::ReplaceCommand(std::string* _appText, Editor* _edit,
	int _pos, int _length, const std::string& _text)
	: Command(_appText, _edit, _pos, _length, _text) {}


bool ReplaceCommand::execute() {
	if (text == "") return false;
	else {
		saveBackUp();
		return editor->replaceText(pos, length, text);
	}
}

//
PasteCommand::PasteCommand(std::string* _appText, Editor* _edit,
	int _pos, const std::string& _text)
	: Command(_appText, _edit, _pos, 0, _text) {}


bool PasteCommand::execute() {
	if (text == "") return false;
	else {
		saveBackUp();
		return editor->insertText(pos, text);
	}
}


//
UndoCommand::UndoCommand(std::string* _appText, Editor* _edit, MacroCommand* _macro)
	: Command(_appText, _edit, 0, 0), macroCmd(_macro) {}


bool UndoCommand::execute() {
	if (macroCmd->cmdStack.empty()) return false;
	else {
		auto cmd = macroCmd->pop();
		cmd->undo();
		delete cmd;
		cmd = nullptr;
		return true;
	}
}


