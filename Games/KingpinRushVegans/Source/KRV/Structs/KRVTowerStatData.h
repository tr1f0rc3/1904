// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVEnums.h"
#include "Structs/KRVAttackStat.h"
#include "KRVTowerStatData.generated.h"

USTRUCT(BlueprintType)
struct FKRVTowerStatData
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		ERangedMediumType RangedMediumType = ERangedMediumType::Arrow;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		TSubclassOf<UKRVDamageType> Ranged_DamageType = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		int32 Ranged_MinDamage = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		int32 Ranged_MaxDamage = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		float Ranged_AttackRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		float Ranged_PenetrationRate = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Ranged")
		int32 Ranged_AttackRange = 1000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Splash")
		TSubclassOf<UKRVDamageType> Splash_DamageType = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Splash")
		int32 Splash_MinDamage = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Splash")
		int32 Splash_MaxDamage = 2;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Splash")
		float Splash_AttackRate = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Splash")
		float Splash_PenetrationRate = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Splash")
		int32 Splash_AttackRange = 1000;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Sight")
		int32 Sight = 1500;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Recruit")
		int32 WaypointRadius = 1500.f;

};
