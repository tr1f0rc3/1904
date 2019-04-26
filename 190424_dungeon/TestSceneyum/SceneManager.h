#pragma once

class SceneManager
{
public:

	enum class eKey
	{
		None = 0,

		Left,
		Up,
		Right,
		Down,

		Action,
	};

	enum class eScene
	{
		None,

		Intro,
		Game,
	};

#pragma region SINGLETON

private:
	static inline SceneManager * m_pInstance = nullptr;
	SceneManager() = default;
	~SceneManager();
	
	SceneManager operator=(SceneManager&) = delete;	
	SceneManager(SceneManager&&) = delete;
	SceneManager(const SceneManager&) = delete;

public:

	static void CreateInstance()
	{
		if (m_pInstance == nullptr)
		{
			m_pInstance = new SceneManager();
		}
	}

	static SceneManager * GetInstance()
	{
		return m_pInstance;
	}

	static void ReleaseInstance()
	{
		SAFE_DELETE(m_pInstance);
	}


#pragma  endregion SINGLETON

public:
	
	void Init();
	void ChangeScene(eScene a_eScene);

	void Update();
	void Render();

	bool KeyInput(char c);

	static eKey GetKey_withChar(char c);

	static void LogError(const char* pLog);

private:

	class Scene * m_pNowScene = nullptr;
};


#define InitSceneMng()		{SceneManager::CreateInstance(); SceneManager::GetInstance()->Init();}
#define SceneMng()			(SceneManager::GetInstance())
#define ReleaseSceneMng()	(SceneManager::ReleaseInstance())

#define Log(x)				{SceneManager::GetInstance()->LogError((x));}

