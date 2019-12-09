// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVEnums.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVProjectileDataAsset.generated.h"

class USoundCue;
class UParticleSystem;

UCLASS()
class KRV_API UKRVProjectileDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Medium")
		ERangedMediumType RangedMediumType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body")
		TSoftObjectPtr<UStaticMesh> BodyMesh;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body")
		float BodyMeshScale = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Body")
		FRotator BodyRotation = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fly")
		TSoftObjectPtr<UParticleSystem> HeadParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fly")
		TSoftObjectPtr<UParticleSystem> TailParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fly")
		TSoftObjectPtr<USoundCue> FlySound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fly")
		bool bRotationFollowsVelocity = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fly")
		FRotator FlyRotationVelocity = FRotator::ZeroRotator;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit")
		TSoftObjectPtr<UParticleSystem> HitParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit")
		TSoftObjectPtr<USoundCue> HitSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit")
		TSoftObjectPtr<UParticleSystem> OnDestroyedParticle;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
		float DeadBodyLifeSpan = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Explode")
		float TimeToExplode = 0.f;

};
