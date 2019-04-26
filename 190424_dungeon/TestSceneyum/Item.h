#pragma once
#include "BaseObject.h"

class Item : public BaseObject
{
public:
	Item(Map* map, int x, int y);
	~Item();

	// BaseObject을(를) 통해 상속됨
	virtual eObjectType GetType() override;
};

