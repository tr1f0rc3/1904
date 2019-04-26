#include "pch.h"
#include "Scene.h"
#include "SceneMaster.h"


Scene::Scene(int _ID) : ID(_ID) { getInfo(); }

bool Scene::changeScene(int _id) {
	return SceneMaster::getInstance()->setScene(_id);
}
