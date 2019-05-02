#include "pch.h"
#include "Seller.h"

Seller::Seller(map<int, int> _menu, map<int, int> _stock, int _dCost, int _disc) :
	menu(_menu), deliveryCost(_dCost), Trader(_disc, _stock) {}

map<int, int> Seller::getMenu() const { return menu; }

bool Seller::Sell(int* _discBuyer, int _num, int _stock, int* _discMarket, float _tax) {
	assert(_tax <= 1 || _tax >= 0);	
	auto it1 = menu.find(_num);
	auto it2 = stock.find(_num);

	if (it1 == menu.end()) {
		cout << "The product number is not found" << endl;
		return false;
	}
	else if (it2 == stock.end()) {
		cout << "Out of stock" << endl;
		return false;
	}
	else if (it2->second <= 0) {
		cout << "Out of stock" << endl;
		return false;
	}
	else if ((*_discBuyer) < _stock * it1->second + deliveryCost) {
		cout << "Buyer need more money" << endl;
		return false;
	}
	else {
		--(it2->second);
		(*_discBuyer) -= (_stock * it1->second);
		if (_discMarket != nullptr) {
			(*_discMarket) += static_cast<int>(_tax * _stock * it1->second);
		}
		else { _tax = 0; }
		discount += static_cast<int>((1 - _tax) * _stock * it1->second) + deliveryCost;
		return true;
	}
}

