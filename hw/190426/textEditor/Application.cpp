#include "pch.h"
#include "Application.h"

Application::~Application() {}


bool Application::addEditor(Editor* _editor) {
	if (_editor == nullptr) return false;
	else {
		editors.emplace_back(_editor);
		return true;
	}
}

bool Application::setEditor(int n) {
	if (n < 0 || n >= editors.size()) return false;
	else {
		activeEditor = editors[n];
		return true;
	}
}

bool Application::executeCmd(Command* _cmd) {
	if (_cmd->execute()) {
		cmdHistory.push(_cmd);
		return true;
	}
	return false;
}

bool Application::copy(int _pos, int _length) {
	CopyCommand* tempCmd 
		= new CopyCommand(&clipBoard, activeEditor, _pos, _length);
	return executeCmd(tempCmd);
}

bool Application::cut(int _pos, int _length) {
	CutCommand* tempCmd
		= new CutCommand(&clipBoard, activeEditor, _pos, _length);
	return executeCmd(tempCmd);
}

bool Application::replace(int _pos, int _length) {
	ReplaceCommand* tempCmd
		= new ReplaceCommand(&clipBoard, activeEditor, _pos, _length, clipBoard);
	return executeCmd(tempCmd);
}

bool Application::paste(int _pos) {
	PasteCommand* tempCmd
		= new PasteCommand(&clipBoard, activeEditor, _pos, clipBoard);
	return executeCmd(tempCmd);
}

bool Application::undo() {
	UndoCommand* tempCmd
		= new UndoCommand(&clipBoard, activeEditor, &cmdHistory);
	return executeCmd(tempCmd);
}
