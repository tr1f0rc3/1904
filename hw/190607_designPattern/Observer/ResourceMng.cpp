
#include "pch.h"
#include "ResourceMng.h"
#include "Scene.h"
#include "SceneMng.h"
#include "Data.h"

ResourceMng* ResourceMng::instance() {
	if (inst == nullptr) {
		inst = new ResourceMng();
	}
	return inst;
}

ResourceMng* ResourceMng::inst = nullptr;


void ResourceMng::update() {
	if (sceneMng != nullptr) {
		int index = sceneMng->getScene()->getEsceneIndex();
		loadData(index);
		sceneMng->getScene()->data = getSceneData();
	}
}

void ResourceMng::loadData(int index) {
	if (dataMap != nullptr) {
		sceneData = dataMap->getData(index);
	}
}

const std::vector<std::string>* ResourceMng::getSceneData() const {
	return &sceneData;
}





