#pragma once


// Replace created obj pointer to chracter pointer
// to avoid compile error 
class CharacterCmd : public Command {
public:
	CharacterCmd(DungeonMaster* _editor, Character* _recv)
		: Command(_editor, _recv) {}
	Character* getReceiver() const { if (receiver != nullptr) return receiver; }
protected:
	Character* receiver = nullptr;
};

// alone-action
class MoveCmd : public CharacterCmd {
public:
	MoveCmd(DungeonMaster* _editor, Character* _recv, Pos _pos, int _dist = 1)
		: CharacterCmd(_editor, _recv), direction(_pos) {}
	Pos getDirection() const { return direction; }
	void execute() override { receiver->move(direction); }
	void undo() override { receiver->move(direction*(-1)); }
protected:
	Pos direction;
};

// class for dmg and healing
class GetAmountCmd : public CharacterCmd {
public:
	GetAmountCmd(DungeonMaster* _editor, Character* _recv, int _amount)
		: CharacterCmd(_editor, _recv), amount(_amount) {}
	int getAmount() const { return amount; }
protected:
	Character* receiver = nullptr;
	int amount = 0;
};

class GetDmgCmd : public GetAmountCmd {
public:
	GetDmgCmd(DungeonMaster* _editor, Character* _recv, int _amount)
		: GetAmountCmd(_editor, _recv, _amount) {}
	void execute() override { receiver->getDamage(amount); }
	void undo() override { (receiver->status).gauge.life += amount; }
};

class GetHealedCmd : public GetAmountCmd {
public:
	GetHealedCmd(DungeonMaster* _editor, Character* _recv, int _amount)
		: GetAmountCmd(_editor, _recv, _amount) {}
	void execute() override { receiver->getHealed(amount); }
	void undo() override { (receiver->status).gauge.life -= amount; }
};

class ResourceChangeCmd : public GetAmountCmd {
public:
	ResourceChangeCmd(DungeonMaster* _editor, Character* _recv, int _amount)
		: GetAmountCmd(_editor, _recv, _amount) {}
	void execute() override { (receiver->status).gauge.resource += amount; }
	void undo() override { (receiver->status).gauge.resource -= amount; }
};

class DieCmd : public Command {
	DieCmd(DungeonMaster* _editor, Character* _recv)
		: Command(_editor, _recv) {}
	void execute() override { receiver->die(); }
	void undo() override { receiver->revive(); }
protected:
	Character* receiver = nullptr;
};

// interaction between characters
class AttackCmd : public TargetCmd {
public:
	AttackCmd(DungeonMaster* _editor, Character* _recv, Character* _target)
		: TargetCmd(_editor, _recv, _target) {}
	void execute() override {
		srand(randomSeed);
		receiver->attack(target);
	}
	void undo() override { }
protected:
	Character* receiver = nullptr;
};

// interaction between a character and a created object
class GetFromCreatedCmd : public CharacterCmd {
public:
	GetFromCreatedCmd(DungeonMaster* _editor, Character* _recv
		, CreatedObj* _createdObj)
		: CharacterCmd(_editor, _recv), createdObj(_createdObj) {}
	CreatedObj* getCreatedObj() const { return createdObj; }
protected:
	CreatedObj* createdObj = nullptr;
};

class DefenceCmd : public GetFromCreatedCmd {
public:
	DefenceCmd(DungeonMaster* _editor, Character* _recv, CreatedAttack* _attack)
		: GetFromCreatedCmd(_editor, _recv, _attack) {}
	void execute() override {
		srand(randomSeed);
		receiver->defence(createdObj);
	}
	void undo() override { }
protected:
	CreatedAttack* createdObj = nullptr;
};

class GetBuffCmd : public GetFromCreatedCmd {
public:
	GetBuffCmd(DungeonMaster* _editor, Character* _recv
		,CreatedBuff* _buff)
		: GetFromCreatedCmd(_editor, _recv, _buff) {}
	void execute() override { receiver->getBuff(createdObj); }
	void undo() override { receiver->removeBuff(createdObj); }
protected:
	CreatedBuff* createdObj = nullptr;
};
