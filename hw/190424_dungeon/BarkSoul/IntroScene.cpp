#include "pch.h"
#include "IntroScene.h"


IntroScene::IntroScene(SceneManager * a_pParent) : Scene{ a_pParent } { }
IntroScene::~IntroScene() { }
SceneManager::eScene IntroScene::GetScene() { return SceneManager::eScene::Intro; }

void IntroScene::Init() {

}
void IntroScene::Update() { 

	if (sceneCnt >=3)
	SceneMng()->ChangeScene(SceneManager::eScene::Game);
}

void IntroScene::Render() {
	system("cls");
	switch (sceneCnt) {
	case 1:
		std::cout << "Long long ago..." << std::endl << std::endl;
		std::cout << "Press y..." << std::endl;
		break;
	case 2:
		std::cout << "Bark Soul" << std::endl << std::endl;
		std::cout << "Grom Software 2019" << std::endl << std::endl;
		std::cout << "Press y to start" << std::endl;
		break;
	default:
		break;
	}
}

void IntroScene::KeyInput(SceneManager::eKey a_eKey)
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
