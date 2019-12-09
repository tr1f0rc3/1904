// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "KRVEnums.h"
#include "Objects/KRVDamageType.h"
#include "KRVAttackStat.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVAttackStat
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		TSubclassOf<UKRVDamageType> DamageType = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float MinDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
		float MaxDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage || Scale")
		float DamageScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackRate")
		float AttackRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AttackRate || Scale")
		float AttackRateScale = 0.f;

	// If attack type is splash, it means splash damage area radius.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range")
		float AttackRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Range || Scale")
		float AttackRangeScale = 0.f;

	FORCEINLINE float GetRandDamage()
	{
		return float(FMath::RandRange(MinDamage, MaxDamage));

	}

	FORCEINLINE FKRVAttackStat& operator=(const FKRVAttackStat& Other)
	{
		DamageType = Other.DamageType;
		MinDamage = Other.MinDamage;
		MaxDamage = Other.MaxDamage;
		DamageScale = Other.DamageScale;
		AttackRate = Other.AttackRate;
		AttackRateScale = Other.AttackRateScale;
		AttackRange = Other.AttackRange;
		AttackRangeScale = Other.AttackRangeScale;
		return *this;

	}

	FORCEINLINE FKRVAttackStat& operator+=(const FKRVAttackStat& Other)
	{
		MinDamage += Other.MinDamage;
		MaxDamage += Other.MaxDamage;
		DamageScale += Other.DamageScale;
		AttackRate += Other.AttackRate;
		AttackRateScale += Other.AttackRateScale;
		AttackRange += Other.AttackRange;
		AttackRangeScale += Other.AttackRangeScale;
		return *this;

	}

	FORCEINLINE FKRVAttackStat operator+(const FKRVAttackStat& Other) const
	{
		FKRVAttackStat NewStat = *this;
		NewStat += Other;
		return NewStat;

	}

	FORCEINLINE FKRVAttackStat& operator*=(const FKRVAttackStat& Other)
	{
		MinDamage *= (1 + Other.DamageScale);
		MaxDamage *= (1 + Other.DamageScale);
		AttackRate *= (1 + Other.AttackRateScale);
		AttackRange *= (1 + Other.AttackRangeScale);
		return *this;

	}

};

