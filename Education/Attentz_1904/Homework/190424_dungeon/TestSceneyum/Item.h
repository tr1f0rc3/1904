#pragma once
#include "BaseObject.h"

class Item : public BaseObject
{
public:
	Item(Map* map, int x, int y);
	~Item();

	// BaseObject��(��) ���� ��ӵ�
	virtual eObjectType GetType() override;
};

