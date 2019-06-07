#include "pch.h"
#include "Slime.h"

Slime::Slime(int _speed) : speed(_speed) {};


Slime::~Slime()
{
}

AcidSlime::AcidSlime(int _speed, int _ph) : Slime(_speed), ph(_ph) {};

