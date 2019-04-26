#pragma once
#include "Scene.h"


class SceneMaster
{

private:
	SceneMaster() = default;
	virtual ~SceneMaster();
	SceneMaster(const SceneMaster&) = delete;
	SceneMaster(SceneMaster&&) = delete;
	inline static SceneMaster* instance = nullptr;
public:
	SceneMaster* getInstance();
public:
	bool addScene(Scene* _scene);
	bool setScene(int id);
	bool currSceneInfo() const;
	bool AllSceneInfo() const;





protected:
	std::map<int, Scene*> scenes;
	Scene* currScene = nullptr;
};

