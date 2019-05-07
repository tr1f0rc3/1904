#include <iostream>
#include <cassert>
#include <vld.h>
#include <map>

#define SAFE_DELETE(x)      { if((x)!= nullptr) { delete (x); (x) = nullptr; } }
#define SAFE_DELETE_ARR(x)  { if((x)!= nullptr) { delete[] (x); (x) = nullptr; } }
#define EXPECT_EQ(x,y)      { assert((x)==(y) && "not same"); }
using namespace std;


class Dealer {

public:
	Dealer() = default;
	Dealer(int _deposit) : deposit(_deposit) {}

	virtual Dealer& operator+=(const pair<int, int>& _stock);
	virtual Dealer& operator-=(const pair<int, int>& _stock);
	virtual bool deal(Dealer*, const pair<int, int>& _stock, int perPrice) = 0;

	int deposit = 0;
	map<int, int> stock = { 0,0 };
};

inline Dealer& operator+=(const pair<int, int>& _stock) {
	stock[_stock.first] += _stock.second;
	return *this;
}

inline Dealer& operator-=(const pair<int, int>& _stock) {
	stock[_stock.first] -= _stock.second;
	return *this;
}


class Buyer : public Dealer {

public:
	virtual bool deal(Dealer* _dealer, const pair<int, int>& _stock, int perPrice) override;
};

class Seller : public Dealer {

public:
	virtual bool deal(Dealer* _dealer, const pair<int, int>& _stock, int perPrice) override;
};

bool Buyer::deal(Dealer* _dealer, pair<int, int> _stock, int perPrice) {

	int sellerRemain = _dealer->stock[_stock.first];
	if (sellerRemain < _stock.second) {
		std::cout << "out of stock" << std::endl;
		return false;
	}
	int netPrice = perPrice * _stock.first;
	if (deposit < netPrice) {
		std::cout << "low of budget" << std::endl;
		return false;
	}

	stock[_stock.first] += _stock.second;
	_dealer[_stock.first] -= _stock.second;
	deposit -= netPrice;
	_dealer->deposit += netPrice;
};

bool Seller::deal(Dealer* _dealer, pair<int, int> _stock, int perPrice) {

	if (stock[_stock.first] < _stock.second) {
		std::cout << "out of stock" << std::endl;
		return false;
	}
	int netPrice = perPrice * _stock.first;
	if (_dealer->deposit < netPrice) {
		std::cout << "low of budget" << std::endl;
		return false;
	}

	stock[_stock.first] -= _stock.second;
	_dealer[_stock.first] += _stock.second;
	deposit += netPrice;
	_dealer->deposit -= netPrice;
};


int main() {


}