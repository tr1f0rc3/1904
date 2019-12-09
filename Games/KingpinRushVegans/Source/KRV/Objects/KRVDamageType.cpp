// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVDamageType.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Pawns/KRVCharacter.h"

UKRVDamageType::UKRVDamageType()
{
	DamageImpulse = 0.f;

}
