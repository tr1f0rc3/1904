#pragma once
#include "Warrior.h"

class WarriorBuilder;

class AxeBuilder {
public:
	AxeBuilder(WarriorBuilder* _parent) : parent(_parent) {};

	inline Axe* make() {
		axe = new Axe();
		return axe;
	}
	inline AxeBuilder* setDamage(int n) {
		assert(axe != nullptr);
		axe->damage = n;
		return this;
	}
	inline AxeBuilder* setAttackSpeed(float n) {
		assert(axe != nullptr);
		axe->attackSpeed = n;
		return this;
	}

	WarriorBuilder* parent = nullptr;

protected:
	Axe* axe = nullptr;
};

class HelmetBuilder {
public:
	HelmetBuilder(WarriorBuilder* _parent) : parent(_parent) {};

	inline Helmet* make() {
		helmet = new Helmet();
		return helmet;
	}
	inline HelmetBuilder* setDefence(int n) {
		assert(helmet != nullptr);
		helmet->defence = n;
		return this;
	}

	WarriorBuilder* parent = nullptr;

protected:
	Helmet* helmet = nullptr;
};


class ShieldBuilder {
public:
	ShieldBuilder(WarriorBuilder* _parent) : parent(_parent) {};

	inline Shield* make() {
		shield = new Shield();
		return shield;
	}
	inline ShieldBuilder* setDefence(int n) {
		assert(shield != nullptr);
		shield->defence = n;
		return this;
	}

	WarriorBuilder* parent = nullptr;

protected:
	Shield* shield = nullptr;
};


class WarriorBuilder
{
public:
	WarriorBuilder();
	~WarriorBuilder();

	inline WarriorBuilder* newWarrior() {
		warrior = new Warrior();
		return this;
	}
	inline Warrior* getWarrior() {
		return warrior;
	}
	inline AxeBuilder* makeAxe() {
		warrior->axe = axeBuilder->make();
		return axeBuilder;
	}
	inline HelmetBuilder* makeHelmet() {
		warrior->helmet = helmetBuilder->make();
		return helmetBuilder;
	}
	inline ShieldBuilder* makeShield() {
		warrior->shield = shieldBuilder->make();
		return shieldBuilder;
	}

protected:
	class Warrior* warrior;
	AxeBuilder* axeBuilder;
	HelmetBuilder* helmetBuilder;
	ShieldBuilder* shieldBuilder;

};


//
//class LegendaryAttackerBuilder : public WarriorBuilder {
//public:
//	virtual void makeEquip() override;
//	virtual class Axe* makeAxe() override;
//};
//
//class AncientDefenderBuilder : public WarriorBuilder {
//public:
//	virtual void makeEquip() override;
//	virtual class Shield* makeShield() override;
//};