#pragma once

class Map;

class Hero;
class BaseObject
{
public:

	BaseObject(Map* map, int x, int y);
	virtual ~BaseObject();

	virtual eObjectType GetType() = 0;
	Map* map;
	Point pos;

	///
	int attackDamage = 0;
	int life = 0;
	int exp = 0;
	virtual void Damage(BaseObject* attacker, int dmg);
	void Attack(BaseObject* attacker, BaseObject* oppo, int dmg);
	virtual void Die(BaseObject* attacker) {};
};



