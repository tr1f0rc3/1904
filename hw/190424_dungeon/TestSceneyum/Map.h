#pragma once

class Map
{
public:
	Map();
	~Map();

	const static int x = 10;
	const static int y = 10;

	void Render();
	void KeyInput(SceneManager::eKey a_eKey);

	class Hero* pHero = nullptr;
	std::vector< std::vector<class BaseObject*>> vcObject;

private:

	char GetChar(class BaseObject* pObj);
	class BaseObject* MakeBaseObject(int n, int x, int y);
	void Print();
	

	string data;


};
