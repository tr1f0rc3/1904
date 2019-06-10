#pragma once
class Gem
{
public:
	Gem(int _radius) : radius(_radius) {};
	Gem();
	~Gem();
	int radius = 0;
	virtual inline int shine() {
		return radius;
	}
};

class Emerald : public Gem {
public:

};

class GemForWand {
	int radius = 0;
	virtual inline int wandShine() = 0;
};

class GemForWandAdapter : public GemForWand, private Gem {
public: 
	GemForWandAdapter(int _radius) : Gem(_radius) {};
	virtual inline int wandShine() override {
		return 1.5 * Gem::shine();
	}
};

class GemForWandAdapter2 : public GemForWand {
public:
	Gem* gem;
	virtual inline int wandShine() override {
		if (gem != nullptr) {
			return 1.5 * gem->shine();
		}
	}
};
