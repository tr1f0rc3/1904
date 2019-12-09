// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/DamageType.h"
#include "KRVEnums.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"
#include "KRVDamageType.generated.h"

struct FHitResult;
class AKRVCharacter;

UCLASS()
class KRV_API UKRVDamageType : public UDamageType
{
	GENERATED_BODY()

public:
	UKRVDamageType();

public: // types
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		EDamageReductionType DamageReductionType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		EDamageEffectType DamageEffectType;

};
