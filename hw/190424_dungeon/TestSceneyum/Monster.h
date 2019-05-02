#pragma once
#include "BaseObject.h"
#include "Hero.h"

class Monster : public BaseObject
{
public:
	Monster(Map* map, int x, int y);
	~Monster();

	// BaseObject을(를) 통해 상속됨
	virtual eObjectType GetType() override;
	virtual void Die(BaseObject* phero) override;

};

