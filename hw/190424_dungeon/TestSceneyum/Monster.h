#pragma once
#include "BaseObject.h"
#include "Hero.h"

class Monster : public BaseObject
{
public:
	Monster(Map* map, int x, int y);
	~Monster();

	// BaseObject��(��) ���� ��ӵ�
	virtual eObjectType GetType() override;
	virtual void Die(BaseObject* phero) override;

};

