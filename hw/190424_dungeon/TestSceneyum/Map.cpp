#include "pch.h"
#include "Map.h"

#include "BaseObject.h"
#include "Wall.h"
#include "Monster.h"
#include "Item.h"
#include "Hero.h"

Map::Map()
{
	data = {
		"1111111111"
		"1000100001"
		"1010401001"
		"1010401021"
		"1010101401"
		"1010101001"
		"1010101041"
		"1010101001"
		"1410001301"
		"1111111111" };

	for (int i = 0; i < Map::y; ++i)
	{	
		vector<BaseObject*> lineVec;
		for (int j = 0; j < Map::x; ++j)
		{
			int nIndex = i * Map::x + j;
			int nType = data[nIndex] - '0';

			auto pObject = MakeBaseObject(nType, j, i);

			if (pObject == nullptr) { continue; }

			if (nType == (int)eObjectType::Hero)
			{
				if (pHero != nullptr) { Log("error - data setting"); }
				pHero = (Hero*)pObject;
			}
			else lineVec.emplace_back(pObject);
		}
		vcObject.push_back(lineVec);
	}
}

Map::~Map()
{
	if (pHero != nullptr)
	{
		SAFE_DELETE(pHero);
	}

	for (auto elem : vcObject)
	{
		for (auto pObj : elem) {
			SAFE_DELETE(pObj);
		}
	}
}

BaseObject* Map::MakeBaseObject(int n, int x, int y)
{
	switch ((eObjectType)n)
	{
	case eObjectType::Wall:
		return new Wall(this, x, y);
		break;
	case eObjectType::Item:
		return new Item(this, x, y);
		break;
	case eObjectType::Hero:
		return new Hero(this, x, y);
		break;
	case eObjectType::Monster:
		return new Monster(this, x, y);
		break;
	}

	return nullptr;
}

char Map::GetChar(BaseObject* pObj)
{
	if (pObj == nullptr) { return ' '; }

	switch (pObj->GetType())
	{
	case eObjectType::Wall:
		return 'W';
		break;
	case eObjectType::Item:
		return 'I';
		break;
	case eObjectType::Hero:
		return 'H';
		break;
	case eObjectType::Monster:
		return 'M';
		break;
	}

	return ' ';
}

void Map::Print()
{	
	for (int i = 0; i != vcObject.size(); ++i) {
		
		auto aLine = vcObject[i];

		std::string sline;
		int oldX(-1), newX(0);
		for (auto& pObj : aLine) {
			char c = GetChar(pObj);
			newX = pObj->pos.x;
			if (newX - oldX - 1 >= 0) {
				sline += std::string(2 * (newX - oldX - 1), ' ') + ' ' + c;
			}
			oldX = newX;
		}
		if (pHero->pos.y == i) {
			sline.replace(2 * (pHero->pos.x), 2, " H");
		}

		std::cout << sline << std::endl;
	}
	std::cout << "  life: " << pHero->life << "  atk: " << pHero->attackDamage << " + " << pHero->weaponDamage
		<< '(' << pHero->durability << ')' << "  lev: " << pHero->level << "  exp: " << pHero->exp << std::endl;


}

void Map::Render()
{
	system("cls");
	Print();
}

void Map::KeyInput(SceneManager::eKey a_eKey)
{	
	std::vector< std::vector<class BaseObject*>>* pvcObject = &vcObject;
	switch (a_eKey)
	{
		case SceneManager::eKey::Up:
		{
			pHero->Move(0,-1, pvcObject);
		} break;
		case SceneManager::eKey::Left:
		{
			pHero->Move(-1,0, pvcObject);
		} break;
		case SceneManager::eKey::Down:
		{
			pHero->Move(0,1, pvcObject);
		} break;
		case SceneManager::eKey::Right:
		{
			pHero->Move(1,0, pvcObject);
		} break;
		default:
			break;
	}
}
