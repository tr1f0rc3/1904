#pragma once

class Scene;

class SceneMaster
{

private:
	SceneMaster();
	virtual ~SceneMaster();
	SceneMaster(const SceneMaster&) = delete;
	SceneMaster(SceneMaster&&) = delete;
	inline static SceneMaster* instance = nullptr;

public:
	static SceneMaster* getInstance();

public:
	bool createAndSetScene(int _id);
	bool setScene(int id);
	bool currSceneInfo() const;
	bool allSceneInfo() const;

	void getInput();

	enum class keyInput {
		none, up, down, left, right, yes, no, quit,
	};



protected:
	bool createScene(int i);
	bool addScene(Scene* _scene);

	std::map<int, Scene*> scenes;
	Scene* currScene = nullptr;
};

