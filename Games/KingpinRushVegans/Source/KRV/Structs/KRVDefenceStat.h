// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRVDefenceStat.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVDefenceStat
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health || Scale")
		float MaxHealthScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthRegenInRest = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health || Scale")
		float HealthRegenInRestScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float HealthRegenInCombat = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health || Scale")
		float HealthRegenInCombatScale = 0.f;

	// percentage to reduce physical damage. 100 armor means does not take physical damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
		float Armor = 0;

	// percentage to reduce magical damage. 100 armor means does not take magical damage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor || Scale")
		float ArmorScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor")
		float MagicResistance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Armor || Scale")
		float MagicResistanceScale = 0.f;

	FORCEINLINE FKRVDefenceStat& operator=(const FKRVDefenceStat& Other)
	{
		MaxHealthScale = Other.MaxHealthScale;
		MaxHealth = Other.MaxHealth;
		HealthRegenInRest = Other.HealthRegenInRest;
		HealthRegenInRestScale = Other.HealthRegenInRestScale;
		HealthRegenInCombat = Other.HealthRegenInCombat;
		HealthRegenInCombatScale = Other.HealthRegenInCombatScale;
		Armor = Other.Armor;
		ArmorScale = Other.ArmorScale;
		MagicResistance = Other.MagicResistance;
		MagicResistanceScale = Other.MagicResistanceScale;
		return *this;

	}

	FORCEINLINE FKRVDefenceStat& operator+=(const FKRVDefenceStat& Other)
	{
		MaxHealthScale += Other.MaxHealthScale;
		MaxHealth += Other.MaxHealth;
		HealthRegenInRest += Other.HealthRegenInRest;
		HealthRegenInRestScale += Other.HealthRegenInRestScale;
		HealthRegenInCombat += Other.HealthRegenInCombat;
		HealthRegenInCombatScale += Other.HealthRegenInCombatScale;
		Armor += Other.Armor;
		ArmorScale += Other.ArmorScale;
		MagicResistance += Other.MagicResistance;
		MagicResistanceScale += Other.MagicResistanceScale;
		return *this;

	}

	FORCEINLINE FKRVDefenceStat operator+(const FKRVDefenceStat& Other) const
	{
		FKRVDefenceStat NewStat = *this;
		NewStat += Other;
		return NewStat;

	}

	FORCEINLINE FKRVDefenceStat& operator*=(const FKRVDefenceStat& Other)
	{
		MaxHealth *= (1 + Other.MaxHealthScale);
		HealthRegenInRest *= (1 + Other.HealthRegenInRestScale);
		HealthRegenInCombat *= (1 + Other.HealthRegenInCombatScale);
		Armor *= (1 + Other.ArmorScale);
		MagicResistance *= (1 + Other.MagicResistanceScale);
		return *this;

	}

};