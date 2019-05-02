#include "pch.h"
#include "Editor.h"

std::string Editor::getText(int _pos, int _length) const {
	assert(_length > 0);
	auto totalLength = text.length();
	if (totalLength - 1 < _pos) {
		return "";
	}
	else if (totalLength < _pos + _length) {
		return text.substr(_pos, totalLength - _pos);
	}
	else return text.substr(_pos, _length);
}

bool Editor::deleteText(int _pos, int _length) {
	if (_pos < 0 || _length <= 0) { return false; }
	auto totalLength = text.length();
	if (totalLength < _pos + _length) {
		text.erase(_pos, totalLength - _pos);
	}
	else { text.erase(_pos, _length); }
	return true;
}

bool Editor::replaceText(int _pos, int _length, const std::string& _text) {
	if (_pos < 0 || _length <= 0) { return false; }
	auto totalLength = text.length();
	if (totalLength < _pos + _length) {
		text.replace(_pos, totalLength - _pos, _text);
	}
	else { text.replace(_pos, _length, _text); }
	return true;
}

bool Editor::insertText(int _pos, const std::string& _text) {
	if (_pos < 0 || _pos >= text.length() || _text == "") { return false; }
	else {
		text.insert(_pos, _text);
		return true;
	}
}
