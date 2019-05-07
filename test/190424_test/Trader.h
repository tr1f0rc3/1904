#pragma once
class Trader
{
public:
	Trader() = default;
	Trader(int _disc);
	Trader(int _disc, map<int, int>);

	virtual ~Trader() {}
	int getDiscount() const;
	map<int, int> getstock() const;
	int discount = 0;

protected:
	map<int, int> stock;
};

