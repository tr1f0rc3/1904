#pragma once

template <typename T>
struct GaugeStats {
	GaugeStats operator+(const GaugeStats&& _input) {
		GaugeStats temp = *this;
		temp += _input;
		return temp;
	}
	GaugeStats operator-(const GaugeStats&& _input) {
		GaugeStats temp = *this;
		temp -= _input;
		return temp;
	}
	GaugeStats<int> operator*(const GaugeStats<float>&& _input) {}
	GaugeStats<int>& operator*=(const GaugeStats<float>& _input) {}
	GaugeStats& operator+=(const GaugeStats& _input);
	GaugeStats& operator-=(const GaugeStats& _input);
	T maxLife = 0;
	T maxResource = 0;
	T lifeRegen = 0;
	T resourceRegen = 0;
};

template <>
GaugeStats<int> GaugeStats<int>::operator*(const GaugeStats<float>&& _input) {
	GaugeStats temp = *this;
	temp *= _input;
	return temp;
}

template <>
GaugeStats<int>& GaugeStats<int>::operator*=(const GaugeStats<float>& _input) {
	maxLife *= _input.maxLife;
	maxResource *= _input.maxResource;
	lifeRegen *= _input.lifeRegen;
	resourceRegen *= _input.resourceRegen;
	static_cast<int>(std::round(maxLife));
	static_cast<int>(std::round(maxResource));
	static_cast<int>(std::round(lifeRegen));
	static_cast<int>(std::round(resourceRegen));
	return *this;
}

template <typename T>
GaugeStats<T>& GaugeStats<T>::operator+=(const GaugeStats& _input) {
	maxLife += _input.maxLife;
	maxResource += _input.maxResource;
	lifeRegen += _input.lifeRegen;
	resourceRegen += _input.resourceRegen;
}

template <typename T>
GaugeStats<T>& GaugeStats<T>::operator-=(const GaugeStats& _input) {
	maxLife -= _input.maxLife;
	maxResource -= _input.maxResource;
	lifeRegen -= _input.lifeRegen;
	resourceRegen -= _input.resourceRegen;
}

