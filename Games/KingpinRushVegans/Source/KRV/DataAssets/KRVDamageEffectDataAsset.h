// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "KRVEnums.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVDamageEffectDataAsset.generated.h"

struct FHitResult;
class AKRVCharacter;

UCLASS()
class KRV_API UKRVDamageEffectDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
		EDamageEffectType DamageEffectType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit")
		TSoftObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit")
		TSoftObjectPtr<USoundCue> HitSound;
	
};
