#include "pch.h"
#include "Wall.h"


Wall::Wall(Map* map, int x, int y) : BaseObject(map, x, y)
{
}


Wall::~Wall()
{
}

eObjectType Wall::GetType()
{
	return eObjectType::Wall;
}
