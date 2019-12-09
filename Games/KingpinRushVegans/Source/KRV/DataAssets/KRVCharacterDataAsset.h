// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVCombatUnitDataAsset.h"
#include "KRVEnums.h"
#include "Structs/KRVDefenceStat.h"
#include "KRVCharacterDataAsset.generated.h"

UCLASS()
class KRV_API UKRVCharacterDataAsset : public UKRVCombatUnitDataAsset
{
	GENERATED_BODY()

public: // Melee
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee")
		TArray<TSoftObjectPtr<UKRVWeaponDataAsset>> LeftMeleeWeapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Melee")
		TArray<TSoftObjectPtr<UKRVWeaponDataAsset>> RightMeleeWeapon;

public: // Hit
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Hit")
		EArmorType ArmorType;

public: // Movement
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Move")
		bool bIsFlying = false;

};



