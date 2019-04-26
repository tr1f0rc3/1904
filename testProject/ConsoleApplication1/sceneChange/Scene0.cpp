#include "pch.h"
#include "Scene0.h"
#include "SceneMaster.h"





bool Scene0::init() {}
bool Scene0::update() {

	switch (keyInput) {
	case 'w':
		SceneMaster::getInstance()->setScene(1);
		break;
	default:
		break;
	}
}
bool Scene0::render() const {
	std::cout << "press w to go to scene 2"
}
