#include "pch.h"
#include "Market.h"


bool Market::addSeller(Seller* _seller) {
	if (_seller == nullptr) { return false; }
	else { sellerList.insert(_seller); }
	return true;
}
bool Market::removeSeller(Seller* _seller) {
	if (_seller == nullptr) { return false; }
	Seller* finded = findSeller(_seller);
	if (finded == nullptr) { return false; }
	else { sellerList.erase(finded); }
	return true;
}
Seller* Market::findSeller(Seller* _seller) const {
	if (_seller == nullptr) { return nullptr; }
	auto it = sellerList.find(_seller);
	if (it == sellerList.end()) { return nullptr; }
	else return (*it);
}
bool Market::addBuyer(Buyer* _buyer) {
	if (_buyer == nullptr) { return false; }
	else { buyerList.insert(_buyer); }
	return true;
}
bool Market::removeBuyer(Buyer* _buyer) {
	if (_buyer == nullptr) { return false; }
	Buyer* finded = findBuyer(_buyer);
	if (finded == nullptr) { return false; }
	else { buyerList.erase(finded); }
	return true;
}
Buyer* Market::findBuyer(Buyer* _buyer) const {
	if (_buyer == nullptr) { return nullptr; }
	auto it = buyerList.find(_buyer);
	if (it == buyerList.end()) { return nullptr; }
	else return (*it);
}