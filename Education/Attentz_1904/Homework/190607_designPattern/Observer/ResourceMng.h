#pragma once
#include "pch.h"

//enum class SceneMng::EScene;

class ResourceMng {

	ResourceMng() {}
	ResourceMng(const ResourceMng&) {}
	virtual ~ResourceMng() {}

public:
	static ResourceMng* instance();

	void update();
	
	class SceneMng* sceneMng = nullptr;
	class Data* dataMap = nullptr;

protected:
	void loadData(int index);
	const std::vector<std::string>* getSceneData() const;

	std::vector<std::string> sceneData;

private:
	static ResourceMng* inst;
};

