#include "pch.h"
#include "Sword.h"


Sword::Sword()
{
}

Sword::~Sword()
{
}

void Sword::onHit() {
	std::cout << "SwordOnHit" << std::endl;
}

void GreatSword::onHit() {
	std::cout << "GreatSwordOnHit" << std::endl;
}

SwordMan::SwordMan() {
	//createSword();
}

SwordMan::~SwordMan() {
	delete sword;
	sword = nullptr;
}

void SwordMan::createSword() {
	sword = new Sword();
	std::cout << "sword created" << std::endl;
}

void SwordMan::slash() {
	sword->onHit();
}


GreatSwordMan::GreatSwordMan() {
	//createSword();
}

GreatSwordMan::~GreatSwordMan() {

}

void GreatSwordMan::createSword() {
	sword = new GreatSword();
	std::cout << "great sword created" << std::endl;

}



