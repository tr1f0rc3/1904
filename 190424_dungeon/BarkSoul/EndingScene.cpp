#include "pch.h"
#include "EndingScene.h"

EndingScene::EndingScene(SceneManager * a_pParent) : Scene{ a_pParent } { }
EndingScene::~EndingScene() { }

SceneManager::eScene EndingScene::GetScene() { return SceneManager::eScene::Game; }

void EndingScene::Init() { }
void EndingScene::Update() {
	if (sceneCnt >= 2)
		SceneMng()->ChangeScene(SceneManager::eScene::Intro);
}
void EndingScene::Render() {
	system("cls");
	switch (sceneCnt) {
	case 1:
		std::cout << "Thanks for playing" << std::endl << std::endl;
		std::cout << "Press y..." << std::endl;
		break;
	default:
		break;
	}
}
void EndingScene::KeyInput(SceneManager::eKey a_eKey)
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


