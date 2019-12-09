#include "HealDmg.h"
#include <iostream>

void Combat::heal(const int amount, int& hp)
{
	hp += amount;
	log(amount, hp);

}

void Combat::dmg(const int amount, int& hp)
{
	if (hp < amount) {
		hp = 0;
	}
	else {
		hp -= amount;
	}
	log(amount, hp);
}

void Combat::log(const int amount, const int& hp) {

	std::cout << "heal: " << amount << " hp: " << hp << std::endl;

}

CombatCommand::CombatCommand(Combat* cs, CombatFcn cf) : combatSystem(cs), combatFcn(cf) {}

CombatCommand::CombatCommand(const CombatCommand& cc) : combatSystem(cc.combatSystem), combatFcn(cc.combatFcn) {};


void CombatCommand::execute(int amount, int& hp) const {
	(combatSystem->*combatFcn)(amount, hp);
	if (!commands.empty()) {
		for (auto& elem : commands) {
			elem->execute(amount, hp);
		}
	}
}

void CombatCommand::addCmd(CombatCommand* cc) {
	commands.emplace_back(cc);
}
