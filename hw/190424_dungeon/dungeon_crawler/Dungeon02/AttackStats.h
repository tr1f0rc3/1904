#pragma once

template <typename T>
struct AttackStats {
	AttackStats& operator+(const AttackStats&& _input) {
		AttackStats temp = *this;
		temp += _input;
		return temp;
	}
	AttackStats& operator-(const AttackStats&& _input) {
		AttackStats temp = *this;
		temp -= _input;
		return temp;
	}
	AttackStats<int> operator*(const AttackStats<float>&& _input) {}
	AttackStats<int>& operator*=(const AttackStats<float>& _input) {}
	AttackStats& operator+=(const AttackStats& _input);
	AttackStats& operator-=(const AttackStats& _input);
	T attackDamage = 0;
	T spellPower = 0;
	float accuracy = 0;
	float crit = 0;
};

template <>
AttackStats<int> AttackStats<int>::operator*(const AttackStats<float>&& _input) {
	AttackStats temp = *this;
	temp *= _input;
	return temp;
}

template <>
AttackStats<int>& AttackStats<int>::operator*=(const AttackStats<float>& _input) {
	attackDamage *= _input.attackDamage;
	spellPower *= _input.spellPower;
	if (_input.accuracy != 0) accuracy *= _input.accuracy;
	if (_input.crit != 0) crit *= _input.crit;
	static_cast<int>(std::round(attackDamage));
	static_cast<int>(std::round(spellPower));
	return *this;
}

template <typename T>
inline AttackStats<T>& AttackStats<T>::operator+=(const AttackStats& _input) {
	attackDamage += _input.attackDamage;
	spellPower += _input.spellPower;
	attackRange += _input.attackRange;
	accuracy += _input.accuracy;
	crit += _input.crit;
}

template <typename T>
inline AttackStats<T>& AttackStats<T>::operator-=(const AttackStats& _input) {
	attackDamage -= _input.attackDamage;
	spellPower -= _input.spellPower;
	accuracy -= _input.accuracy;
	crit -= _input.crit;
}