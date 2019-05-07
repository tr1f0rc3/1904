#pragma once

#include "Scene.h"
#include "SceneManager.h"

class IntroScene : public Scene
{
public:

	IntroScene(SceneManager * a_pParent);
	virtual ~IntroScene();

	virtual SceneManager::eScene GetScene();

	virtual void Init();
	virtual void Update();
	virtual void Render();
	virtual void KeyInput(SceneManager::eKey a_eKey);

public:
	int sceneCnt = 0;
	class SceneManager * m_refParent = nullptr;
};
