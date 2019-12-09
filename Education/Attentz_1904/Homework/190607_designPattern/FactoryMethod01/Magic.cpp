#include "pch.h"
#include "Magic.h"

FireMagic::FireMagic()
{
}


FireMagic::~FireMagic()
{
}


FireMagic* MagicFactory::factory(MagicType magicType) {
	switch (magicType)
	{
	case MagicType::Fire: {
		auto newMagic = new FireMagic();
		return newMagic;
		break;
	}
	case MagicType::HellFire: {
		auto newMagic = new HellFireMagic();
		return newMagic;
		break;
	}
	default: {
		break;
	}
	}
	return nullptr;
}