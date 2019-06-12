#include "pch.h"
#include "MagicOrb.h"


MagicOrb::MagicOrb(int _fire, int _ice) : fire(_fire), ice(_ice) {}

MagicOrb::~MagicOrb() {}

void MagicOrb::activate() {
	std::cout << "fire: " << fire << " ice: " << ice << std::endl;
}

DecoratedMagicOrb::DecoratedMagicOrb(MagicOrb* _magicOrb) {
	magicOrb = _magicOrb;
}

FireEnhancedOrb::FireEnhancedOrb(MagicOrb* _magicOrb) : DecoratedMagicOrb(_magicOrb) {}

void FireEnhancedOrb::activate() {

	std::cout << "enhanced fire: " << 1.5 * magicOrb->fire << std::endl;
	DecoratedMagicOrb::activate();
}
