#pragma once
#include "BaseObject.h"

class Wall : public BaseObject
{
public:
	Wall(Map* map, int x, int y);
	~Wall();

	// BaseObject��(��) ���� ��ӵ�
	virtual eObjectType GetType() override;
};

