#include "pch.h"
#include "SceneManager.h"


int main()
{
	using namespace std;

	InitSceneMng();

	while (true)
	{
		char c;
		cin >> c;

		if (std::cin.fail())
		{
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			continue;
		}

		if (c == 'Q')
		{
			cout << "quit" << endl;
			break;
		}

		std::cin.ignore(10000, '\n');

		if (SceneMng()->KeyInput(c) == true)
		{
			SceneMng()->Update();
			SceneMng()->Render();
		}
	}

	ReleaseSceneMng();

	return 0;
}
