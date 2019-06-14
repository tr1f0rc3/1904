#include "pch.h"
#include "Scene.h"
#include "SceneMng.h"
#include "ResourceMng.h"


SceneMng* SceneMng::instance() {
	if (inst == nullptr) {
		inst = new SceneMng();
	}
	return inst;
}

SceneMng* SceneMng::inst = nullptr;

Scene* SceneMng::getScene() const {
	return scene;
}


void SceneMng::changeScene(EScene escene) {
	
	Scene* newScene = nullptr;
	int index = static_cast<int>(escene);

	switch (escene)
	{
	case SceneMng::EScene::none:
		break;
	case SceneMng::EScene::title:
		newScene = new Scene(escene);
		break;
	case SceneMng::EScene::loading:
		newScene = new Scene(escene);
		break;
	case SceneMng::EScene::game:
		newScene = new Scene(escene);
		break;
	default:
		break;
	}

	if (newScene != nullptr) {
		delete scene;
		scene = newScene;
	}

	notify();
}

void SceneMng::notify() const {
	resourceMng->update();
}
