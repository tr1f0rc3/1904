

#include "pch.h"
#include "Scene1.h"
#include "SceneMaster.h"


Scene1::Scene1() : Scene(1) {}


void Scene1::getInfo() const {
	std::cout << "this is scene 1" << std::endl;
}

bool Scene1::init() {
	sc0Cnt = 0;
	return true;
}

bool Scene1::getInput(SceneMaster::keyInput _input) {
	switch (_input)
	{
	case SceneMaster::keyInput::none:
		return false;
		break;
	case SceneMaster::keyInput::up:
		break;
	case SceneMaster::keyInput::down:
		break;
	case SceneMaster::keyInput::left:
		break;
	case SceneMaster::keyInput::right:
		break;
	case SceneMaster::keyInput::yes:
		--sc0Cnt;
		break;
	case SceneMaster::keyInput::no:
		++sc0Cnt;
		break;
	default:
		return false;
		break;
	}
	return true;
};

bool Scene1::update() {

	switch (sc0Cnt)
	{
	case 0:
		printText = "press n 2 times to go to the scene 0";
		break;
	case 1:
		printText = "press n 1 times to go to the scene 0";
		break;
	case 2:
		changeScene(0);
		break;
	default:
		break;
	}
	return true;
}

bool Scene1::render() const {
	std::cout << printText << std::endl;
	return true;
}
