#pragma once

class SceneMaster;

class Scene
{
public:
	Scene();
	virtual ~Scene() {}

	virtual void getInfo() const = 0;
	int getID() const { return ID; }

	virtual bool init() = 0;
	virtual bool update() = 0;
	virtual bool render() const = 0;
	virtual bool getInput(char _c);

	//bool changeScene(int id) {
	//	SceneMaster::getInstance()->
	//}

	SceneMaster* master;

protected:
	int ID;
	char keyInput = 'a';
};

