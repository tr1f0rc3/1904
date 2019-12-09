#include "pch.h"
#include "GameScene.h"

#include <cassert>

#include "Map.h";

GameScene::GameScene(SceneManager * a_pParent) : Scene {a_pParent} { }
GameScene::~GameScene()
{
	SAFE_DELETE(map);
}

SceneManager::eScene GameScene::GetScene() const { return SceneManager::eScene::Game;  }

void GameScene::Init()
{
	map = new Map();
}

void GameScene::Update() { }
int GameScene::Render()
{
	map->Render();

	return Scene::Render();
}
void GameScene::KeyInput(SceneManager::eKey a_eKey)
{
	map->KeyInput(a_eKey);
}
