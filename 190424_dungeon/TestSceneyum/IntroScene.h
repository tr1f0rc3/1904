#pragma once

#include "Scene.h"
#include "SceneManager.h"

class IntroScene : public Scene
{
	enum class eUI
	{
		Start,
		End,

		Max,
	};

public:

	IntroScene(SceneManager * a_pParent);
	virtual ~IntroScene();

	virtual SceneManager::eScene GetScene() const;

	virtual void Init();
	virtual void Update();
	virtual int Render();
	virtual void KeyInput(SceneManager::eKey a_eKey);

	

private:

	void DoAction(eUI a_eUI);

private:

	int m_nUIIndex = 0;
};
