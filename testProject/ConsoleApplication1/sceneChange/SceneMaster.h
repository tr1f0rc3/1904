#pragma once

class Scene;

class SceneMaster
{

private:
	SceneMaster() = default;
	virtual ~SceneMaster();
	SceneMaster(const SceneMaster&) = delete;
	SceneMaster(SceneMaster&&) = delete;
	inline static SceneMaster* instance = nullptr;

public:
	static SceneMaster* getInstance();

public:
	bool createScene(int i);
	bool setScene(int id);
	bool currSceneInfo() const;
	bool allSceneInfo() const;

	void getInput();





protected:
	bool addScene(Scene* _scene);

	std::map<int, Scene*> scenes;
	Scene* currScene = nullptr;
};

