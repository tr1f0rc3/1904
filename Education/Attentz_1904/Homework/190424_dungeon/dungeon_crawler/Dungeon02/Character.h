#pragma once


#ifndef BUFF_TYPE
#define BUFF_TYPE
enum class BuffType {
	none,
	inherited,
	gear, consumable,
	bless, magic, poison, curse,
	size,
};
#endif


struct Status {
	Gauge gauge;
	CreatedBuffComposite buffs;
	std::array<bool, static_cast<int>(BuffType::size)> buffMask = { false, };
	// crowd control
};
struct CreatedAttack;

class Character : public CreatedObj {
public:
	void move(Pos _dir);
	CreatedAttack* attack(Character* _target);
	void defence(CreatedAttack* _attack);
	void getDamage(int _damage);
	void getHealed(int _heal);
	void getBuff(CreatedBuff* _createdBuff);
	void removeBuff(CreatedBuff* _createdBuff);
	virtual void die() = 0;
	virtual void revive() = 0;

	Pos position;
	Status status;
	Stats<int> baseStats;
	Stats<int> totalStats;

	int attackCnt = 0;
	int getDmgCnt = 0;
};


CreatedAttack* Character::attack(Character* _target1) {
	if (!isOccured(totalStats.attackStats.accuracy)) {
		return nullptr;
	}

	if (isOccured(totalStats.attackStats.crit)) {

	}
	else {}


}

struct CreatedAttack : public CreatedObj {

};


