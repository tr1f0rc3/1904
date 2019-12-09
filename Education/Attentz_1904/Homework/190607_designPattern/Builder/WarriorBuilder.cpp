#include "pch.h"
#include "WarriorBuilder.h"
#include "Warrior.h"

WarriorBuilder::WarriorBuilder()
{
	axeBuilder = new AxeBuilder(this);
	helmetBuilder = new HelmetBuilder(this);
	shieldBuilder = new ShieldBuilder(this);
}

WarriorBuilder::~WarriorBuilder()
{
}

//void LegendaryAttackerBuilder::makeEquip() {
//	assert(warrior != nullptr);
//	warrior->axe = makeAxe();
//	warrior->helmet = makeHelmet();
//
//}
//
//
//void AncientDefenderBuilder::makeEquip() {
//	assert(warrior != nullptr);
//	warrior->axe = makeAxe();
//	warrior->helmet = makeHelmet();
//	warrior->shield = makeShield();
//
//
//}
//
//
//
//Axe* LegendaryAttackerBuilder::makeAxe() {
//	return new LegendaryAxe();
//}
//
//Shield* AncientDefenderBuilder::makeShield() {
//	return new AncientShield();
//}
