#include "pch.h"
#include "BaseObject.h"

BaseObject::BaseObject(Map* map, int x, int y) : pos(x, y), map(map)
{
}

BaseObject::~BaseObject()
{
}


void BaseObject::Damage(BaseObject* attacker, int dmg) {
	life -= dmg;
	if (life <= 0) Die(attacker);
}

void BaseObject::Attack(BaseObject* attacker, BaseObject* oppo, int dmg) {
	oppo->Damage(attacker, dmg);
};


