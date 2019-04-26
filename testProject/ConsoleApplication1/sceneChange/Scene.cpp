#include "pch.h"
#include "Scene.h"
#include "SceneMaster.h"




Scene::Scene() { getInfo(); }


bool Scene::getInput(char _c) {
	keyInput = _c;
	return true;
}
