#include "pch.h"

#include "SceneManager.h"
#include "Scene.h"

#include "IntroScene.h"
#include "GameScene.h"
#include "GameOverScene.h"
#include "EndingScene.h"




SceneManager::eKey SceneManager::GetKey_withChar(char c)
{
	switch (c)
	{
		case 'w':
		{
			return eKey::Up;
		} break;
		case 'a':
		{
			return eKey::Left;
		} break;
		case 's':
		{
			return eKey::Down;
		} break;
		case 'd':
		{
			return eKey::Right;
		} break;
		case 'y':
		{
			return eKey::Yes;
		} break;
		case 'n':
		{
			return eKey::No;
		} break;
		default:
		{
			return eKey::None;
		} break;
	}
}

SceneManager::~SceneManager()
{
	SAFE_DELETE(m_pNowScene);
}

void SceneManager::Init()
{
	ChangeScene(eScene::Intro);
}

void SceneManager::ChangeScene(eScene a_eScene)
{
	if ( m_pNowScene != nullptr &&
		m_pNowScene->GetScene() == a_eScene)
	{ return;	}
	SAFE_DELETE(m_pNowScene);
	
	switch(a_eScene)
	{
		case eScene::Intro:
		{
			m_pNowScene = new IntroScene(this);
		} break;
		case eScene::Game:
		{
			m_pNowScene = new GameScene(this);
		} break;
		case eScene::GameOver:
		{
			m_pNowScene = new GameOverScene(this);
		} break;
		case eScene::Ending:
		{
			m_pNowScene = new EndingScene(this);
		} break;
	}

	m_pNowScene->Init();
}

void SceneManager::Update()
{
	m_pNowScene->Update();
}

void SceneManager::Render()
{
	m_pNowScene->Render();
}

bool SceneManager::KeyInput(char c)
{
	eKey e = GetKey_withChar(c);

	if (e == eKey::None) { return false; }

	m_pNowScene->KeyInput(e);
	return true;
}

