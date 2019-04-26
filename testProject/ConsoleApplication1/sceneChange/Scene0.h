#pragma once
#include "Scene.h"

class Scene0
{
public:
	virtual void getInfo() const = 0;
	int getID() const { return ID; }

	virtual bool init();
	virtual bool update();
	virtual bool render() const;

	//bool changeScene(int id) {
	//	SceneMaster::getInstance()->
	//}

	SceneMaster* master;
	int ID;
};
