// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVEnums.h"
#include "Structs/KRVAttackStat.h"
#include "KRVCombatUnitDataAsset.generated.h"

class USoundCue;
class UParticleSystem;
class UAnimInstance;
class USkeletalMesh;
class UKRVWeaponDataAsset;
class UKRVShooterComponent;
class UKRVSummonerComponent;
class UKRVShooterComponentDataAsset;
class UKRVAnimInstance;
class AKRVSkill;

UCLASS(Abstract)
class KRV_API UKRVCombatUnitDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public: // Ranged
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
		ERangedMediumType RangedMediumType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
		TArray<TSoftObjectPtr<UKRVWeaponDataAsset>> LeftRangedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
		TArray<TSoftObjectPtr<UKRVWeaponDataAsset>> RightRangedWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
		TArray<TSoftObjectPtr<UKRVShooterComponentDataAsset>> ShooterComponent;

public: // Skills
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Skill")
		TArray<TSubclassOf<AKRVSkill>> SkillClass;

public:	// OnSpawn and OnDestroy
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
		TSoftObjectPtr<USoundCue> OnSpawnedSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Spawn")
		TSoftObjectPtr<UParticleSystem> OnSpawnedParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Spawn")
		TSoftObjectPtr<UParticleSystem> OnDestroyedParticle;

public: // Death
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
		TSoftObjectPtr<USoundCue> DeathSound;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death")
		TSoftObjectPtr<UParticleSystem> DeathParticle;

public: // Animation
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Anim")
		TSoftObjectPtr<class UKRVAnimInstanceDataAsset> AnimInstanceDataAsset;

//
//public: // Meshes and anims 
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
//		TSoftObjectPtr<USkeletalMesh> BodySkeletalMesh;
//
//	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Mesh")
//		TSubclassOf<UKRVAnimInstance> BodyAnimInstance;

};
