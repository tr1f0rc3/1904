#include "pch.h"

std::vector<Pos> Radius::radiusToPos(int _radius) {
	// find and return
	auto it = radiusToPosDict.find(_radius);
	if (it != radiusToPosDict.end()) {
		return it->second;
	}
	// init and detect sign
	std::vector<Pos> vec;
	vec.push_back(Pos(0, 0));
	if (_radius == 0) { 
		radiusToPosDict.insert({ 0,vec });
		return vec;
	}
	else if (_radius < 0) { std::negate<int>(_radius); }
	// create Pos in cross (+)
	for (int i = 1; i <= _radius; ++i) {
		vec.push_back(Pos(i, 0));
		vec.push_back(Pos(-i, 0));
		vec.push_back(Pos(0, i));
		vec.push_back(Pos(0, -i));
	}
	// create Pos in diamond except cross
	for (int j = _radius; j != 0; --j) {
		for (int i = 1; i < j; ++i) {
			vec.push_back(Pos(i, j - i));
			vec.push_back(Pos(-i, j - i));
			vec.push_back(Pos(i, i - j));
			vec.push_back(Pos(-i, i - j));
		}
	}
	// create Pos out of diamond
	for (int i = 1; i <= _radius; ++i) {
		for (int j = _radius; (_radius - j) < i ; --j) {
			if ((i * i + j * j) <= _radius * _radius) {
				for (int k = j; i + k > _radius; --k) {
					vec.push_back(Pos(i, k));
					vec.push_back(Pos(-i, k));
					vec.push_back(Pos(i, -k));
					vec.push_back(Pos(-i, -k));
				}
				break;
			}
		}
	}
	// sort vec, add to map and return
	sort(vec.begin(), vec.end());
	radiusToPosDict.insert({ _radius,vec });
	return vec;
}