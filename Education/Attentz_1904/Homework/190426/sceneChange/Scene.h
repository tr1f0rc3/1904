#pragma once

#include "SceneMaster.h"

class Scene
{
public:
	Scene(int ID);
	virtual ~Scene() {}

	virtual void getInfo() const = 0;
	int getID() const { return ID; }

	virtual bool init() = 0;
	virtual bool update() = 0;
	virtual bool render() const = 0;
	virtual bool getInput(SceneMaster::keyInput _input) = 0;
	bool changeScene(int id);
	
	SceneMaster* master = nullptr;

protected:
	int ID;
	SceneMaster::keyInput keyInput = SceneMaster::keyInput::none;
};

