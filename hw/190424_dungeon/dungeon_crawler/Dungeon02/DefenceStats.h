#pragma once

template <typename T>
struct DefenceStats {
	DefenceStats& operator+(const DefenceStats&& _input) {
		DefenceStats temp = *this;
		temp += _input;
		return temp;
	}
	DefenceStats& operator-(const DefenceStats&& _input) {
		DefenceStats temp = *this;
		temp -= _input;
		return temp;
	}
	DefenceStats<int> operator*(const DefenceStats<float>&& _input) {}
	DefenceStats<int>& operator*=(const DefenceStats<float>& _input) {}
	DefenceStats& operator+=(const DefenceStats& _input);
	DefenceStats& operator-=(const DefenceStats& _input);
	T armor = 0;
	T magicResist = 0;
	float evasion = 0;
};

template <>
DefenceStats<int> DefenceStats<int>::operator*(const DefenceStats<float>&& _input) {
	DefenceStats temp = *this;
	temp *= _input;
	return temp;
}

template <>
DefenceStats<int>& DefenceStats<int>::operator*=(const DefenceStats<float>& _input) {
	DefenceStats temp = *this;
	armor *= _input.armor;
	magicResist *= _input.magicResist;
	if (_input.evasion != 0) evasion *= _input.evasion;
	static_cast<int>(std::round(armor));
	static_cast<int>(std::round(magicResist));
	return *this;
}

template <typename T>
inline DefenceStats<T>& DefenceStats<T>::operator+=(const DefenceStats& _input) {
	armor += _input.armor;
	magicResist += _input.magicResist;
	evasion += _input.evasion;
}

template <typename T>
inline DefenceStats<T>& DefenceStats<T>::operator-=(const DefenceStats& _input) {
	armor -= _input.armor;
	magicResist -= _input.magicResist;
	evasion -= _input.evasion;
}