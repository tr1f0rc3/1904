#include "pch.h"
#include "Scene0.h"
#include "SceneMaster.h"


Scene0::Scene0() : Scene(0) {}


void Scene0::getInfo() const {
	std::cout << "this is scene 0" << std::endl;
}

bool Scene0::init() {
	sc0Cnt = 0;
	return true;
}

bool Scene0::getInput(SceneMaster::keyInput _input) {
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
		++sc0Cnt;
		break;
	case SceneMaster::keyInput::no:
		--sc0Cnt;
		break;
	default:
		return false;
		break;
	}
	return true;
};

bool Scene0::update() {

	switch (sc0Cnt)
	{
	case 0:
		printText = "press y 2 times to go to the next scene";
		break;
	case 1:
		printText = "press y 1 times to go to the next scene";
		break;
	case 2:
		changeScene(1);
		break;
	default:
		break;
	}
	return true;
}

bool Scene0::render() const {
	system("cls");
	std::cout << printText << std::endl;
	return true;
}
