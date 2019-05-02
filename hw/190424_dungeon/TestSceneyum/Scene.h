#pragma once

#include "SceneManager.h"

class Scene
{
public:

	Scene(SceneManager * a_pParent);
	virtual ~Scene();

	virtual SceneManager::eScene GetScene() const = 0;

	virtual void Init();
	virtual void Update();
	virtual int Render();

	virtual void KeyInput(SceneManager::eKey a_eKey);

public:

	class SceneManager * m_refParent = nullptr;
};
