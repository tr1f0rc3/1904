#pragma once
class Slime
{
public:
	Slime() = default;
	Slime(int _speed);
	virtual ~Slime();

	int speed = 0;

	inline Slime* clone() {
		return new Slime(*this);
	}
};

class AcidSlime : public Slime {
public:
	AcidSlime() = default;
	AcidSlime(int _speed, int _ph);
	virtual ~AcidSlime() {}

	int ph = 0;

	inline AcidSlime* clone() {
		return new AcidSlime(*this);
	}
};

