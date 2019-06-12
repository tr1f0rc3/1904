#pragma once
class Orb {
	virtual void activate() = 0;
};

class MagicOrb : public Orb {
public:
	MagicOrb() = default;
	MagicOrb(int _fire, int _ice);
	virtual ~MagicOrb();
	
	void activate();

	int fire;
	int ice;
};

class DecoratedMagicOrb : public MagicOrb{
public:
	DecoratedMagicOrb(MagicOrb* _magicOrb);

	inline void activate() {
		magicOrb->activate();
	}


	MagicOrb* magicOrb;
};

class FireEnhancedOrb : public DecoratedMagicOrb {
public:
	FireEnhancedOrb(MagicOrb* _magicOrb);

	void activate();

};