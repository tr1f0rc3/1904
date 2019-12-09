// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "KRVEnums.h"
#include "Structs/KRVAttackStat.h"
#include "Structs/KRVDefenceStat.h"
#include "Objects/KRVDamageType.h"
#include "KRVCharacterStatData.generated.h"

USTRUCT(BlueprintType)
struct FKRVCharacterStatData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Melee")
		TSubclassOf<UKRVDamageType> Melee_DamageType = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee")
		int32 Melee_MinDamage = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee")
		int32 Melee_MaxDamage = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee")
		float Melee_AttackRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Melee")
		int32 Melee_AttackRange = 150;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		ERangedMediumType RangedMediumType = ERangedMediumType::NoWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Ranged")
		TSubclassOf<UKRVDamageType> Ranged_DamageType = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		int32 Ranged_MinDamage = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		int32 Ranged_MaxDamage = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		float Ranged_AttackRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		int32 Ranged_AttackRange = 800;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		int32 Sight = 1500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
		int32 MaxHealth = 5;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
		float HealthRegenInRest = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
		float HealthRegenInCombat = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Armor")
		int32 Armor = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Armor")
		int32 MagicResistance = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Move")
		int32 MovementSpeed = 200;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
		int32 LivesTaken = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
		int32 Bounty = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Reward")
		int32 ExpGiven = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Note")
		FString Notes;

};
