#include "pch.h"
#include "SceneMaster.h"

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
		return true;
	}
}

bool SceneMaster::currSceneInfo() const {
	if (currScene == nullptr) return false; {
		currScene->getInfo();
		return true;
	}
}
bool SceneMaster::AllSceneInfo() const {
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