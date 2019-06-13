#pragma once
#include <vector>

class Combat {
public:
	void heal(const int amount, int& hp);
	void dmg(const int amount, int& hp);
protected:
	void log(const int amount, const int& hp);
};

class CombatCommand {
	
	using CombatFcn = void (Combat::*)(const int, int&);

public:
	CombatCommand(Combat* cs, CombatFcn cf);
	CombatCommand(const CombatCommand& cc);

	void execute(int amount, int& hp) const;
	void addCmd(CombatCommand* cc);

protected:
	Combat* combatSystem;
	CombatFcn combatFcn;
	std::vector<CombatCommand*> commands;
	
};
