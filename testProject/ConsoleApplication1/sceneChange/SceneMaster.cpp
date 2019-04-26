#include "pch.h"
#include "SceneMaster.h"
#include "Scene.h"
#include "Scene0.h"
#include "Scene1.h"

SceneMaster::~SceneMaster() {
	if (scenes.size() != 0) {
		for (auto scene : scenes) {
			if (scene.second != nullptr) {
				delete scene.second;
				scene.second = nullptr;
			}
		}
	}
	if (instance != nullptr) {
		delete instance;
		instance = nullptr;
	}
}

SceneMaster* SceneMaster::getInstance() {
	if (instance == nullptr) {
		instance = new SceneMaster;
	}
	return instance;
}

bool SceneMaster::createScene(int i) {
	Scene* temp = nullptr;
	switch (i) {
	case 0: {
		temp = new Scene0;
		break;
	}
	case 1: {
		temp = new Scene1;
		break;
	}
	default:
		break;
	}
	addScene(temp);
	return true;
}


bool SceneMaster::addScene(Scene* _scene) {
	if (_scene == nullptr) return false;
	else {
		scenes.insert({ _scene->getID(), _scene });
		_scene->master = this;
		return true;
	}
}

bool SceneMaster::setScene(int _id) {
	auto it = scenes.find(_id);
	if (it == scenes.end()) return false;
	else {
		currScene = it->second;
		currScene->init();
		return true;
	}
}

bool SceneMaster::createAndSetScene(int _id) {
	return (createScene(_id) &&
		setScene(_id));
}

bool SceneMaster::currSceneInfo() const {
	if (currScene == nullptr) return false; {
		currScene->getInfo();
		return true;
	}
}

bool SceneMaster::allSceneInfo() const {
	if (scenes.size() == 0) return false;
	else {
		for (auto scene : scenes) {
			if (scene.second == nullptr) return false; {
				scene.second->getInfo();
				return true;
			}
		}
		return true;
	}
}

void SceneMaster::init() {
	createAndSetScene(0);
}

void SceneMaster::getInput() {
	char _input;
	do {
		std::cin >> _input;
	} while (_input);
	SceneMaster::keyInput toInput;
	switch (_input)	{
	case 'w':
		toInput = SceneMaster::keyInput::up;
		break;
	case 's':
		SceneMaster::keyInput::down;
		break;
	case 'a':
		SceneMaster::keyInput::left;
		break;
	case 'd':
		SceneMaster::keyInput::right;
		break;
	case 'y':
		SceneMaster::keyInput::yes;
		break;
	case 'n':
		SceneMaster::keyInput::no;
		break;
	default:
		SceneMaster::keyInput::none;
		break;
	}
	currScene->getInput(toInput);
}
