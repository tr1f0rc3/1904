// FactoryMethod01.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "Sword.h"
#include "Magic.h"
#include "Bow.h"
#include "Car.h"
#include "Fruit.h"

int main()
{
	{
		Fruit fruit;
		global::createFruit(FruitType::Apple, &fruit);
		std::cout << std::endl;
	}
	{
		Car c1;
		CarFactory cf;
		cf.createCar(CarType::Gold, &c1);
		std::cout << std::endl;

	}
	{
		Archer ac1;
		ac1.crateBow(BowType::Default);
		Archer ac2;
		ac2.crateBow(BowType::Long);
		ac1.shoot();
		ac2.shoot();
	}
	{
		MagicFactory mf;
		auto fire = mf.factory(MagicType::Fire);
		auto hellfire = mf.factory(MagicType::HellFire);
		fire->activate();
		hellfire->activate();
	}
	{
		SwordMan sman;
		GreatSwordMan gsman;
		sman.createSword();
		gsman.createSword();
		sman.slash();
		gsman.slash();
	}
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
