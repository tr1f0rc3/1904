#pragma once

#include "Trader.h"
#include "Buyer.h"
#include "Seller.h"


class Seller;
class Buyer;

class Market : public Trader
{
public:
	Market() = default;
	Market(float _tax) : tax(_tax) {}
	bool addSeller(Seller* _seller);
	bool removeSeller(Seller* _seller);
	Seller* findSeller(Seller* _seller) const;
	bool addBuyer(Buyer* _buyer);
	bool removeBuyer(Buyer* _buyer);
	Buyer* findBuyer(Buyer* _buyer) const;
	
	float getTax() const { return tax; }


protected:
	set<Seller*> sellerList;
	set<Buyer*> buyerList;
	float tax = 0.05;
};

