#include "pch.h"
#include "SceneMaster.h"
#include "Scene.h"
#include "Scene0.h"
#include "Scene1.h"

SceneMaster::SceneMaster() {
	createAndSetScene(0);
}

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
	auto it = scenes.find(i);
	if (it != scenes.end()) { return false; }
	else {
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
			return false;
		}
		addScene(temp);
		return true;
	}
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
	createScene(_id);
	return setScene(_id);
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

void SceneMaster::getInput() {
	char _input;
	while (true) {

		std::cin >> _input;
		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			continue;
		}

		SceneMaster::keyInput toInput;
		switch (_input) {
		case 'w':
			toInput = SceneMaster::keyInput::up;
			break;
		case 's':
			toInput=SceneMaster::keyInput::down;
			break;
		case 'a':
			toInput=SceneMaster::keyInput::left;
			break;
		case 'd':
			toInput=SceneMaster::keyInput::right;
			break;
		case 'y':
			toInput=SceneMaster::keyInput::yes;
			break;
		case 'n':
			toInput=SceneMaster::keyInput::no;
			break;
		case 'Q':
			toInput=SceneMaster::keyInput::quit;
			std::cout << "let's quit" << std::endl;
			return;
		default:
			SceneMaster::keyInput::none;
			break;
		}

		currScene->getInput(toInput);
		currScene->update();
		currScene->render();
	}
}
