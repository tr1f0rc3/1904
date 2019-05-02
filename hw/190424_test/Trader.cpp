#include "pch.h"
#include "Trader.h"

Trader::Trader(int _disc) : discount(_disc) {}
Trader::Trader(int _disc, map<int, int> _stock) : discount(_disc), stock(_stock) {}

int Trader::getDiscount() const { return discount; }
map<int, int> Trader::getstock() const { return stock; }
