#include "pch.h"
#include "SceneMaster.h"
#include "Scene.h"

class Scene0;

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
	switch (i) {
	case 0: {
		Scene* temp = new Scene0;
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
	do {
		std::cin >> _input;
	} while (_input);
	currScene->getInput(_input);
}
