#pragma once

#include "Trader.h"

class Buyer : public Trader
{
public:
	Buyer() = default;
	Buyer(int _discount);
	//bool buy();
	int discout = 0;
};

