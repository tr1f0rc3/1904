#include "pch.h"
#include "GameScene.h"


GameScene::GameScene(SceneManager * a_pParent) : Scene{ a_pParent } { }
GameScene::~GameScene() {
	ClearStage();
	SAFE_DELETE(player);


}

SceneManager::eScene GameScene::GetScene() { return SceneManager::eScene::Game; }

void GameScene::Init() {
	Player* temp = new Player(this);
	player = temp;
	LOAD_MAP("stage00.txt", currMap);
	Scan();
}
void GameScene::Update() {
	for (auto& elem : monsters) {
		(elem.second)->myTurn();
	}
	for (auto& elem : items) {
		(elem.second)->myTurn();
	}
	nextStage->myTurn();
}
void GameScene::Render() {
	system("cls");
	enum { screenSize = 40 };
	int xDef = player->pos.x - screenSize / 2;
	int yDef = player->pos.y - screenSize / 2;
	if (xDef < 0) xDef = 0;
	if (yDef < 0) yDef = 0;
	Pos posi;
	std::string aLine;
	for (int i = yDef; i <= yDef+screenSize; ++i) {
		for (int j = xDef; j <= xDef+screenSize; ++j) {
			posi.x = j;
			posi.y = i;
			auto it = find(player->sightArea.begin(), player->sightArea.end(), posi-(player->pos));
			if (it == player->sightArea.end()) {
				aLine += ' ';
			}
			else {
				aLine += currMap[i,j];
			}
		}
		std::cout << aLine << std::endl;
		aLine = "";
	}
	std::cout << " stage: " << stage << "  life: " << player->life << "  atk: " <<
		player->attackDamage << " + " << player->weaponDamage << "  durability: " << player->durability << std::endl;
}
void GameScene::KeyInput(SceneManager::eKey a_eKey)
{
	player->moveCmd(a_eKey);
}

void GameScene::Scan() {
	enum { mapLimit = 200 };
	Pos posi;
	Monster* pmon;
	Item* pitem;
	NextStage* pnext;
	for (int i = 0; i != mapLimit - 1; ++i) {
		for (int j = 0; j != mapLimit - 1; ++j) {
			posi.x = j;
			posi.y = i;
			switch (currMap[i][j]) {
			case 'M': {
				Monster* temp = new Monster(this);
				pmon = temp;
				pmon->pos = posi;
				monsters.insert({ posi, pmon });
				break;
			}
			case 'I': {
				pitem = new Item(this);
				pitem->pos = posi;
				items.insert({ posi, pitem });
				break;
			}
			case 'E': {
				NextStage* temp = new NextStage(this);
				pnext = temp;
				pnext->pos = posi;
				break;
			}
			case 'P': {
				player->pos = posi;
				break;
			}
			default:
				break;
			}
		}
	}
}


void GameScene::ClearStage() {

	if (monsters.size() != 0) {
		for (auto& elem : monsters) {
			SAFE_DELETE(elem.second);
		}
	}
	if (items.size() != 0) {
		for (auto& elem : items) {
			SAFE_DELETE(elem.second);
		}
	}
	SAFE_DELETE(nextStage);
}
