#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "Visitors.h"
#include "Places.h"

int main()
{
	vector<Places*> list =
	{
	  new Inn(string("Cozy")), new Market(string("Rumble")), new ElvenTemple(string("Highborne"))
	};
	list[1]->add(new Market(string("Bear")));
	list[1]->add(new Market(string("Fine")));
	OrcVisitor thrall;
	ElfVisitor Sylvanas;
	for (auto elem : list)
		//    and passes each
		elem->accept(&thrall);
	//    to accept() calls
	for (auto elem : list)
		elem->accept(&Sylvanas);
}