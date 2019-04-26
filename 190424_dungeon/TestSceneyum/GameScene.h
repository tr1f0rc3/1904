#pragma once

#include "Scene.h"
#include "SceneManager.h"

class GameScene : public Scene
{
public:

	GameScene(SceneManager * a_pParent);
	virtual ~GameScene();

	virtual SceneManager::eScene GetScene() const;

	virtual void Init();
	virtual void Update();
	virtual int Render();

	virtual void KeyInput(SceneManager::eKey a_eKey);

	class Map* map = nullptr;
};
