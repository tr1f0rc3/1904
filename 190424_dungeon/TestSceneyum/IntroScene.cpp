#include "pch.h"

#include "IntroScene.h"
#include <cassert>

IntroScene::IntroScene(SceneManager * a_pParent) : Scene{ a_pParent } { }
IntroScene::~IntroScene() { }
SceneManager::eScene IntroScene::GetScene() const { return SceneManager::eScene::Intro; }

void IntroScene::Init() { }
void IntroScene::Update() { }
int IntroScene::Render()
{
	const int nStart = 8;
	const int nEnd = 12;
	const int nX = 8;

	gotoxy(nX, nStart);
	cout << "Start Game" << endl;

	gotoxy(nX, nEnd);
	cout << "End Game" << endl;

	switch ((eUI)m_nUIIndex)
	{
		case eUI::Start: 
		{
			gotoxy(nX-2, nEnd);
			cout << " " << endl;
			gotoxy(nX-2, nStart);
			cout << "&" << endl;
		} break;
		case eUI::End:
		{
			gotoxy(nX - 2, nStart);
			cout << " " << endl;
			gotoxy(nX - 2, nEnd);
			cout << "&" << endl;
		} break;
		default:
		{
			Log("logic error - m_nUIIndex");
		}
	}

	return Scene::Render();
}

void IntroScene::KeyInput(SceneManager::eKey a_eKey)
{
	switch (a_eKey)
	{

	case SceneManager::eKey::Up:
	case SceneManager::eKey::Left:
		++m_nUIIndex;
		m_nUIIndex %= (int)eUI::Max;

		break;

	case SceneManager::eKey::Down:
	case SceneManager::eKey::Right:
		--m_nUIIndex;
		if (m_nUIIndex < 0)
		{
			m_nUIIndex = (int)eUI::Max - 1;
		}
		break;

	case SceneManager::eKey::Action:
		{
			DoAction((eUI)m_nUIIndex);
		}
		break;
	}
}

void IntroScene::DoAction(eUI a_eUI)
{
	switch (a_eUI)
	{
		case IntroScene::eUI::Start:
			SceneMng()->ChangeScene(SceneManager::eScene::Game);
			break;
		case IntroScene::eUI::End:
			exit(0);
			break;
		default:
			Log("arg error");
			break;
	}
}
