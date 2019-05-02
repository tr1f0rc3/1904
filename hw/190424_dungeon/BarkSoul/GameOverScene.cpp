#include "pch.h"
#include "GameOverScene.h"

GameOverScene::GameOverScene(SceneManager * a_pParent) : Scene{ a_pParent } { }
GameOverScene::~GameOverScene() { }

SceneManager::eScene GameOverScene::GetScene() { return SceneManager::eScene::Game; }

void GameOverScene::Init() { }
void GameOverScene::Update() { 
	if (sceneCnt >= 2)
		SceneMng()->ChangeScene(SceneManager::eScene::Intro);
}
void GameOverScene::Render() {
	system("cls");
	switch (sceneCnt) {
	case 1:
		std::cout << "YOU DIED" << std::endl << std::endl;
		std::cout << "Press y..." << std::endl;
		break;
	default:
		break;
	}
}
void GameOverScene::KeyInput(SceneManager::eKey a_eKey)
{
	switch (a_eKey)
	{
	case SceneManager::eKey::Yes:
		++sceneCnt;
		break;
	default:
		break;
	}
}


