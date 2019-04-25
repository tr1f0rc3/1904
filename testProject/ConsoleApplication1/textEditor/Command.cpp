#include "pch.h"
#include "Command.h"

//
Command::Command(std::string* _appText, Editor* _edit, 
	int _pos, int _length)
 : appText(_appText), editor(_edit),
	pos(_pos), length(_length) {}

bool Command::saveBackUp() {
	if (editor == nullptr) { return false; }
	else {
		text = editor->text;
		return true;
	}
}

bool Command::undo() {
	if (editor == nullptr) { return false; }
	else {
		editor->text = text;
		return true;
	}
}


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
	else return editor->deleteText(pos, length);
}

ReplaceCommand::ReplaceCommand(std::string* _appText, Editor* _edit,
	int _pos, int _length)
	: Command(_appText, _edit, _pos, _length) {}


bool ReplaceCommand::execute() {
	if (text == "") return false;
	else return editor->replaceText(pos, length, text);
}

PasteCommand::PasteCommand(std::string* _appText, Editor* _edit,
	int _pos, int _length)
	: Command(_appText, _edit, _pos, _length) {}


bool PasteCommand::execute() {
	if (text == "") return false;
	else return editor->insertText(pos, text);
}



