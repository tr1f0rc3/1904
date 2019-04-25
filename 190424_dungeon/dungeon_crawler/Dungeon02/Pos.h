#pragma once
#include <cmath>

struct Pos
{
	Pos() = default;
	Pos(int _x, int _y) : x(_x), y(_y) {}
	~Pos() {}

	Pos operator+(const Pos& _Pos);
	Pos& operator+=(const Pos& _Pos);
	Pos operator-(const Pos& _Pos);
	Pos& operator-=(const Pos& _Pos);
	Pos operator*(float _i);
	Pos& operator*=(float _i);
	Pos& operator=(const Pos& _Pos);


	Pos& negate();

	int x=0;
	int y=0;
};

inline Pos Pos::operator+(const Pos& _Pos) {
	Pos temp(*this);
	temp += _Pos;
	return temp;
}
inline Pos& Pos::operator+=(const Pos& _Pos) {
	x += _Pos.x;
	y += _Pos.y;
	return *this;
}
inline Pos Pos::operator-(const Pos& _Pos) {
	Pos temp(*this);
	temp -= _Pos;
	return temp;
}
inline Pos& Pos::operator-=(const Pos& _Pos) {
	x -= _Pos.x;
	y -= _Pos.y;
	return *this;
}
inline Pos Pos::operator*(float _i) {
	Pos temp(*this);
	temp *= _i;
	return temp;
}
inline Pos& Pos::operator*=(float _i) {
	float tempX = static_cast<float>(x) * _i;
	float tempY = static_cast<float>(y) * _i;
	x = static_cast<int>(std::round(tempX));
	y = static_cast<int>(std::round(tempY));
	return *this;
}

inline Pos& Pos::operator=(const Pos& _Pos) {
	x = _Pos.x;
	y = _Pos.y;
	return *this;
}


inline Pos& Pos::negate() {
	x *= -1;
	y *= -1;
	return *this;
}
