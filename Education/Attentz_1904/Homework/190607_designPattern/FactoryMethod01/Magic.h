#pragma once



enum class MagicType {
	Fire,
	HellFire
};

class MagicFactory {
public:
	class FireMagic* factory(MagicType magicType);
};

class FireMagic
{
public:
	FireMagic();
	virtual ~FireMagic();
	virtual void activate() {
		std::cout << "fire" << std::endl;
	}
};

class HellFireMagic : public FireMagic
{
public:
	virtual void activate() override {
		std::cout << "hell fire" << std::endl;
	}
};