#include "pch.h"
#include "Command.h"

//
Command::Command(std::string* _appText, Editor* _edit, 
	int _pos, int _length, const std::string& _text)
 : appText(_appText), editor(_edit),
	pos(_pos), length(_length), text(_text) {}

bool Command::saveBackUp() {
	if (editor == nullptr) { return false; }
	else {
		backUp = editor->text;
		return true;
	}
}

bool Command::undo() {
	if (editor == nullptr) { return false; }
	else {
		editor->text = backUp;
		return true;
	}
}

//MacroCommand::MacroCommand(std::string* _appText, Editor* _edit)
//	: Command(_appText, _edit, 0, 0) {}

MacroCommand::~MacroCommand() {
	while (cmdStack.empty()) {
		Command* temp = pop();
		if (temp != nullptr) {
			delete temp;
			temp = nullptr;
		}
	}
}



bool MacroCommand::push(Command* _cmd) {
	if (_cmd == nullptr) { return false; }
	else {
		cmdStack.push(_cmd);
		return true;
	}
}
Command* MacroCommand::pop() {
	if (cmdStack.empty()) { return nullptr; }
	else {
		auto temp = cmdStack.top();
		cmdStack.pop();
		return temp;
	}
}