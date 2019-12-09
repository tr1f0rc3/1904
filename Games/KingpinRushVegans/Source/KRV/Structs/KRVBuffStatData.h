// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "KRVEnums.h"
#include "KRVBuffStatData.generated.h"

USTRUCT(BlueprintType)
struct FKRVBuffStatData : public FTableRowBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Team")
		ETargetType TargetType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Duration")
		int32 SpellLevel = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Duration")
		float Duration = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		int32 MinDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		int32 MaxDamage = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Damage")
		float DamageScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rate")
		float AttackRate = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Rate")
		float AttackRateScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range")
		int32 AttackRange = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Range")
		float AttackRangeScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		int32 MaxHealth = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float MaxHealthScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float HealthRegenInRest = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float HealthRegenInRestScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float HealthRegenInCombat = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Health")
		float HealthRegenInCombatScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor")
		int32 Armor = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor")
		float ArmorScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor")
		int32 MagicResistance = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Armor")
		float MagicResistanceScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
		int32 MovementSpeed = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Move")
		float MovementSpeedScale = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Overtime")
		TSubclassOf<UDamageType> DOTDamageType;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Overtime")
		int32 TotalOvertimeAmount = 0;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Overtime")
		int32 OvertimeTicks = 0;

};
