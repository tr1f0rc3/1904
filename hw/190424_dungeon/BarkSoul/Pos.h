#pragma once
#include <cmath>

struct Pos
{
	Pos() = default;
	Pos(int _x, int _y) : x(_x), y(_y) {}
	Pos(const Pos& _pos) : x(_pos.x), y(_pos.y) {}
	Pos(Pos&& _pos) noexcept : x(_pos.x), y(_pos.y) {}


	~Pos() {}

	Pos operator+(const Pos& _Pos);
	Pos& operator+=(const Pos& _Pos);
	Pos operator-(const Pos& _Pos);
	Pos& operator-=(const Pos& _Pos);
	Pos operator*(const float& _i);
	Pos& operator*=(const float& _i);
	Pos& operator=(const Pos&);
	bool operator==(const Pos& rhs) const {
		return (x == rhs.x) && (y == rhs.y);
	}
	bool operator!=(const Pos& rhs) const {
		return !(*this==rhs);
	}
	bool operator<(const Pos& rhs) const;
	bool operator>(const Pos& rhs) const {
		return !(*this < rhs);
	}
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
inline Pos Pos::operator*(const float& _i) {
	Pos temp(*this);
	temp *= _i;
	return temp;
}
inline Pos& Pos::operator*=(const float& _i) {
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

inline bool Pos::operator<(const Pos& rhs) const {
	if (y < rhs.y) return true;
	else if (y > rhs.y) return false;
	else if (x < rhs.x) return true;
	else return false;
}


inline Pos& Pos::negate() {
	x *= -1;
	y *= -1;
	return *this;
}
