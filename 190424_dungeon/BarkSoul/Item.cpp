#include "pch.h"
#include "GameScene.h"

void Item::myTurn() {
	if (scene->player->pos == pos) {
		int stages = scene->stage;
		if (isOccured(0.7)) {
			(scene->player)->life += 200 + stages * 40 + 20 * (stages * getRandom(3));
		}
		else {
			(scene->player)->weaponDamage = 20 + stages * 5 + 5 * (stages * getRandom(3));
			(scene->player)->durability = 4 + getRandom(6);
		}
		graphic = ' ';

	}
}

void NextStage::myTurn() {
	if (scene->player->pos == pos) {
		++(scene->stage);
		std::string stageFile;
		switch (scene->stage)
		{
		case 1:
			stageFile = "stage01.txt";
			break;
		case 2:
			stageFile = "stage02.txt";
			break;
		case 3:
			stageFile = "stage03.txt";
			break;
		case 4:
			SceneMng()->ChangeScene(SceneManager::eScene::Ending);
			return;
		default:
			break;
		}
		scene->ClearStage();
		LOAD_MAP(stageFile, scene->currMap);
		scene->Scan();
	}
}