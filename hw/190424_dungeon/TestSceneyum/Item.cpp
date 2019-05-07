#include "pch.h"
#include "Item.h"

Item::Item(Map* map, int x, int y) : BaseObject(map, x, y)
{
}

Item::~Item()
{
}

eObjectType Item::GetType()
{
	return eObjectType::Item;
}
