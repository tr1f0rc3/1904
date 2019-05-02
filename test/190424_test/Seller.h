#pragma once
#include "Trader.h"

class Buyer;
class Seller : public Trader
{
public:
	Seller(map<int, int> _menu, map<int, int> _stock, int _dCost = 4000, int _disc = 0);
	
	map<int, int> getMenu() const;
	
	//bool registerProduct(int* _discBuyer, int _num, int* _discMarket, float _tax);
	bool Sell(int* _discBuyer, int _num, int _stock, int* _discMarket = nullptr, float _tax = 0);

private:
	int deliveryCost;
	int discount = 0;
	map<int, int> menu;
};

