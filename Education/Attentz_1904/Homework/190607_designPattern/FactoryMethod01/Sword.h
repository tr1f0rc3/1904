#pragma once

class Sword
{
public:
	Sword();
	virtual ~Sword();
	virtual void onHit();
};

class GreatSword : public Sword
{
public:
	GreatSword() = default;
	virtual ~GreatSword() {}
	virtual void onHit() override;
};


class SwordMan {
	
public:

	SwordMan();
	virtual ~SwordMan();

	virtual void createSword();
	void slash();
	Sword* sword;
};

class GreatSwordMan : public SwordMan {

public:

	GreatSwordMan();
	virtual ~GreatSwordMan();

	virtual void createSword() override;
};

