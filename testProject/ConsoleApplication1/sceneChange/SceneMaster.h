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
	bool createAndSetScene(int _id);
	bool currSceneInfo() const;
	bool allSceneInfo() const;

	void init();
	void getInput();

	enum class keyInput {
		none, up, down, left, right, yes, no, 
	};



protected:
	bool addScene(Scene* _scene);

	std::map<int, Scene*> scenes;
	Scene* currScene = nullptr;
};

