#pragma once

template <typename T>
struct RangeStats {
	RangeStats operator+(const RangeStats&& _input) {
		RangeStats temp = *this;
		temp += _input;
		return temp;
	}
	RangeStats operator-(const RangeStats&& _input) {
		RangeStats temp = *this;
		temp -= _input;
		return temp;
	}
	RangeStats<int> operator*(const RangeStats<float>&& _input) {}
	RangeStats<int>& operator*=(const RangeStats<float>& _input) {}
	RangeStats& operator+=(const RangeStats& _input);
	RangeStats& operator-=(const RangeStats& _input);
	T sightRange = 0;
	T attackRange = 0;
	T aggroRange = 0;
};

template <>
RangeStats<int> RangeStats<int>::operator*(const RangeStats<float>&& _input) {
	RangeStats temp = *this;
	temp *= _input;
	return temp;
}

template <>
RangeStats<int>& RangeStats<int>::operator*=(const RangeStats<float>& _input) {
	sightRange *= _input.sightRange;
	attackRange *= _input.attackRange;
	aggroRange *= _input.aggroRange;
	static_cast<int>(std::round(sightRange));
	static_cast<int>(std::round(attackRange));
	static_cast<int>(std::round(aggroRange));
	return *this;
}

template <typename T>
RangeStats<T>& RangeStats<T>::operator+=(const RangeStats& _input) {
	sightRange += _input.sightRange;
	attackRange += _input.attackRange;
	aggroRange += _input.aggroRange;
}

template <typename T>
RangeStats<T>& RangeStats<T>::operator-=(const RangeStats& _input) {
	sightRange -= _input.sightRange;
	attackRange -= _input.attackRange;
	aggroRange -= _input.aggroRange;
}
