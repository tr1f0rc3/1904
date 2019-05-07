#pragma once
template <typename T> // T should be int or float
struct Gauge {
	// Gauge(T _life, T _resource) : life(_life), resource(_resource) {};
	Gauge operator+(const Gauge&& _input) {
		Gauge temp = *this;
		temp += _input;
		return temp;
	}
	Gauge operator-(const Gauge&& _input) {
		Gauge temp = *this;
		temp -= _input;
		return temp;
	}
	Gauge<int> operator*(const Gauge<float>&& _input) {}
	Gauge<int>& operator*=(const Gauge<float>& _input) {}
	Gauge& operator+=(const Gauge& _input);
	Gauge& operator-=(const Gauge& _input);

	T life = 0;
	T resource = 0;
};

template <>
Gauge<int> Gauge<int>::operator*(const Gauge<float>&& _input) {
	Gauge temp = *this;
	temp *= _input;
	return temp;
}

template <>
Gauge<int>& Gauge<int>::operator*=(const Gauge<float>& _input) {
	life *= _input.life;
	resource *= _input.resource;
	return *this;
}

template <typename T>
Gauge<T>& Gauge<T>::operator+=(const Gauge& _input) {
	life += _input.life;
	resource += _input.resource;
	return *this
}

template <typename T>
Gauge<T>& Gauge<T>::operator-=(const Gauge& _input) {
	life -= _input.life;
	resource -= _input.resource;
	return *this
}