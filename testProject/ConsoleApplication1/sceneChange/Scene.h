#pragma once

class SceneMaster;

class Scene
{
public:
	Scene();
	virtual ~Scene() {}

	virtual void getInfo() const = 0;
	int getID() const { return ID; }

	virtual bool update() = 0;
	virtual bool render() const = 0;
	virtual bool getInput(char _c) = 0;

	//bool changeScene(int id) {
	//	SceneMaster::getInstance()->
	//}

	SceneMaster* master;
	int ID;
};

