#pragma once

class Player : public Character {
public:
	void useItem(CreatedItem* _item);
	void dropItem(CreatedItem* _item);

	int killCnt = 0;
};




