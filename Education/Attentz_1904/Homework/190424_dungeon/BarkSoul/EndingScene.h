#pragma once

#include "Scene.h"
#include "SceneManager.h"

class EndingScene : public Scene
{
public:

	EndingScene(SceneManager * a_pParent);
	virtual ~EndingScene();

	virtual SceneManager::eScene GetScene();

	virtual void Init();
	virtual void Update();
	virtual void Render();

	virtual void KeyInput(SceneManager::eKey a_eKey);

public:

	class SceneManager * m_refParent = nullptr;

public:
	int sceneCnt = 0;
};


