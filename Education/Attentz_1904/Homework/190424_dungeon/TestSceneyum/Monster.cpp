#include "pch.h"
#include "Monster.h"


class Hero;
Monster::Monster(Map* map, int x, int y) : BaseObject(map, x, y)
{
	life = 30;
	attackDamage = 10;
	exp = 1;
}


Monster::~Monster()
{
}

eObjectType Monster::GetType()
{
	if (life > 0) {
		return eObjectType::Monster;
	}
	else if (life <= 0) {
		return eObjectType::DeadBody;
	}
}


void Monster::Die(BaseObject* phero) {
	phero->exp += exp;

}



