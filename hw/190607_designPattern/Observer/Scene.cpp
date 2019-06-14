#include "pch.h"
#include "Scene.h"
#include "ResourceMng.h"

void Scene::play() const {
	for (auto& elem : *data) {
		std::cout << elem << ' ';
	}
	std::cout << std::endl;
}

int Scene::getEsceneIndex() const {
	return static_cast<int>(escene);
}
