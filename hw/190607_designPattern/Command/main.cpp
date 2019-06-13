#include <iostream>
#include <vector>

#include "HealDmg.h"
using namespace std;


int main()
{
	Combat cb;
	CombatCommand cc1(&cb, &(Combat::dmg));
	int hp = 10;
	cc1.execute(2, hp);
	CombatCommand cc2(&cb, &(Combat::heal));
	cc1.addCmd(&cc2);
	cc1.execute(2, hp);
	
}