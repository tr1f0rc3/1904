// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVBuffDataAsset.generated.h"

class UParticleSystem;
class USoundCue;

UCLASS()
class KRV_API UKRVBuffDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particle")
		TSoftObjectPtr<UParticleSystem> BuffParticle;

	// no socketname means particle show up in root position
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particle")
		TArray<FName> ParticleSocketNames;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Particle")
		TSoftObjectPtr<USoundCue> BuffSound;

};
