#pragma once
#include "pch.h"

class Scene;

class SceneMng {

	SceneMng() {}
	SceneMng(const SceneMng&) {}
	virtual ~SceneMng() {}

public:
	enum class EScene {
		none,
		title,
		loading,
		game,
		count,
	};

	static SceneMng* instance();

	Scene* getScene() const;
	void changeScene(EScene escene);

	class ResourceMng* resourceMng = nullptr;

protected:
	void notify() const;
	Scene* scene = nullptr;

private:

	static SceneMng* inst;
};

