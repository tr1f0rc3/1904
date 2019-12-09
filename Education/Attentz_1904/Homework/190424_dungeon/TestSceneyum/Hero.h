#pragma once
#include "BaseObject.h"

class Map;

class Hero : public BaseObject
{
public:
	Hero(Map* map, int x, int y);
	~Hero();
	void Die();
	void Damage(BaseObject* attacker, int dmg) override;
	void Move(int x, int y, std::vector< std::vector<class BaseObject*>>* vcObject);
	// BaseObject을(를) 통해 상속됨
	virtual eObjectType GetType() override;
	int weaponDamage = 0;
	int durability = 0;
	int level=0;
	int nextLevel = 2;
	void LevelUp() {
		++level;
		life += 10;
		attackDamage += 1;
		nextLevel += 2 + level;
	}
};

