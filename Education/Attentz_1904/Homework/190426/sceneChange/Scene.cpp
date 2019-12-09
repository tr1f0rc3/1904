#include "pch.h"
#include "Scene.h"
#include "SceneMaster.h"


Scene::Scene(int _ID) : ID(_ID) { }

bool Scene::changeScene(int _id) {
	return SceneMaster::getInstance()->createAndSetScene(_id);
}
