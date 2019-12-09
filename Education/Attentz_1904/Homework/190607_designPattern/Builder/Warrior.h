#pragma once

class Axe {
	friend class AxeBuilder;
protected:
	int damage = 0;
	float attackSpeed = 0.0;
};
class LegendaryAxe : public Axe {};
class AncientAxe : public Axe {};
class Helmet {
	friend class HelmetBuilder;
protected:
	int defence = 0;
};
class Shield {
	friend class ShieldBuilder;
protected:
	int defence = 0;
};
class LegendaryShield : public Shield {};
class AncientShield : public Shield {};

class Warrior
{
	friend class WarriorBuilder;
public:
	Warrior();
	~Warrior();

	Axe* axe = nullptr;
	Helmet* helmet = nullptr;
	Shield* shield = nullptr;

};

