#pragma once

#define MAKE_FIELD(p, fieldType, subclass)\
{ p->x##fieldType = new subclass##fieldType(); }

#define MAKE_FRUIT(p, subclass)\
{\
	MAKE_FIELD(p, Seed, subclass)\
	MAKE_FIELD(p, Peel, subclass)\
}

enum class FruitType {
	Apple,
	Melon,
};

class Seed {};
class AppleSeed : public Seed {};
class MelonSeed : public Seed {};
class Peel {};
class ApplePeel : public Peel {};
class MelonPeel : public Peel {};


class Fruit {
public:
	Seed* xSeed;
	Peel* xPeel;
};

namespace global {
	inline void createFruit(FruitType fruitType, Fruit* fruit)
	{
			if (fruit == nullptr) {
				return;
			}
			switch (fruitType) {
			case FruitType::Apple: {
				MAKE_FRUIT(fruit, Apple);
				break;
			}
			case FruitType::Melon: {
				MAKE_FRUIT(fruit, Melon);
				break;
			}
			}
	}
}