#pragma once
#include "BaseObject.h"

class Wall : public BaseObject
{
public:
	Wall(Map* map, int x, int y);
	~Wall();

	// BaseObject을(를) 통해 상속됨
	virtual eObjectType GetType() override;
};

