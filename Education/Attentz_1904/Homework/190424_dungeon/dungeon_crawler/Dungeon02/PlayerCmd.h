#pragma once

class ItemCmd;
class UseItemCmd;
class DropItemCmd;

class ItemCmd : public Command {
public:
	ItemCmd(DungeonMaster* _editor, Player* _recv, CreatedItem* _item)
		: Command(_editor, _recv), item(_item) {}
protected:
	Player* receiver = nullptr;
	CreatedItem* item = nullptr;
};

class UseItemCmd : public ItemCmd {
public:
	UseItemCmd(DungeonMaster* _editor, Player* _recv, CreatedItem* _item)
		: ItemCmd(_editor, _recv, _item) {}
	void execute() {
		receiver->useItem(item);
	}
};

class DropItemCmd : public ItemCmd {
public:
	DropItemCmd(DungeonMaster* _editor, Player* _recv, CreatedItem* _item)
		: ItemCmd(_editor, _recv, _item) {}
	void execute() {
		receiver->dropItem(item);
	}
};
