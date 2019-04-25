#include "pch.h"
#include "Application.h"

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


bool Application::copy(int _pos, int _length);
bool Application::cut(int _pos, int _length);
bool Application::replace(int _pos, int _length);
bool Application::paste(int _pos);
bool Application::undo();
