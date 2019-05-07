#pragma once

#include "AttackStats.h"
#include "DefenceStats.h"
#include "GaugeStats.h"
#include "RangeStats.h"
#include "Gauge.h"

template <typename T> // T should be int or float
struct Stats {
	Stats operator+(const Stats&& _input) {
		Stats temp = *this;
		temp += _input;
		return temp;
	}
	Stats operator-(const Stats&& _input) {
		Stats temp = *this;
		temp -= _input;
		return temp;
	}
	Stats<int> operator*(const Stats<float>&& _input) {}
	Stats<int>& operator*=(const Stats<float>& _input) {}
	Stats& operator+=(const Stats& _input);
	Stats& operator-=(const Stats& _input);
	AttackStats<T> attackStats;
	DefenceStats<T> defenceStats;
	GaugeStats<T> gaugeStats;
	RangeStats<T> rangeStats;
};

template <>
Stats<int> Stats<int>::operator*(const Stats<float>&& _input) {
	Stats temp = *this;
	temp *= _input;
	return temp;
}

template <>
Stats<int>& Stats<int>::operator*=(const Stats<float>& _input) {
	attackStats *= _input.attackStats;
	defenceStats *= _input.defenceStats;
	gaugeStats *= _input.gaugeStats;
	rangeStats *= _input.rangeStats;
	return *this;
}

template <typename T>
Stats<T>& Stats<T>::operator+=(const Stats& _input) {
	attackStats += _input.attackStats;
	defenceStats += _input.defenceStats;
	gaugeStats += _input.gaugeStats;
	radiusStats += _input.rangeStats;
	return *this
}

template <typename T>
Stats<T>& Stats<T>::operator-=(const Stats& _input) {
	attackStats -= _input.attackStats;
	defenceStats -= _input.defenceStats;
	gaugeStats -= _input.gaugeStats;
	radiusStats -= _input.rangeStats;
	return *this
}

template <typename T>
struct StatsComposite : public Composite<Stats<T>> {
public:
	Stats<T>& getTotalStats() { if (totalStats != nullptr) return totalStats; }
	Stats<T>& calculateTotalStats();

private:
	Stats<T> totalStats;
};

template <typename T>
inline Stats<T>& StatsComposite<T>::calculateTotalStats() {
	Stats<T> temp;
	for (auto& elem : Composite<Stats<T>>::composite) {
		temp += elem;
	}
	totalStats = temp;
	return totalStats;
}




