#pragma once
#include "pch.h"
#include "SceneMng.h"

class Scene
{
	friend class SceneMng;
	friend class ResourceMng;
public:
	Scene(SceneMng::EScene _escene) : escene(_escene) {}
	int getEsceneIndex() const;
	void play() const;
	
protected:
	SceneMng::EScene escene;
	const std::vector<std::string>* data = nullptr;
};

