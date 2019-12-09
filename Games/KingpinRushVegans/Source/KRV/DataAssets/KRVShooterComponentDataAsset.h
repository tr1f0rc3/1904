// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVShooterComponentDataAsset.generated.h"

class UParticleSystem;
class USoundCue;
class AKRVProjectile;

UCLASS()
class KRV_API UKRVShooterComponentDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
		TSoftObjectPtr<UParticleSystem> FireParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
		TSoftObjectPtr<USoundCue> FireSound;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
		FVector FireLocation = FVector(20.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire")
		FRotator FireRotation = FRotator(0.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		TSubclassOf<AKRVProjectile> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		int32 MaxProjectileInAir = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float InitialProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float MaxProjectileSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float HomingAcceleration;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile")
		float GravityScale = 0.f;
	
};
