#pragma once


class Item {
public:
};

class CreatedItem {
public:
	Character* user = nullptr;
	int useCnt = 0;
	int coolDown = 0;
	Item* info = nullptr;
	bool isUsable = true;
};
