// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "KRVEnums.h"
#include "KRVTowerUpgradeDataAsset.generated.h"

UCLASS()
class KRV_API UKRVTowerUpgradeDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public: // Genreral
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		ETowerUpgradeType TowerUpgradeType = ETowerUpgradeType::NewTower;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UKRVTowerUpgradeDataAsset> NextTowerUpgradeDA;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 UpgradeCost = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 UpgradeLevelTo = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 MaxUpgradeLevel = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float BuildTime = 3.f;

public: // Upgrade to NewTower
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class AKRVTower> NewTower;

public:	// Unit & Tower Stat upgrade
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class AKRVBuff> UpgradeStat;

public: // Unit & Tower Skill upgrade
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class AKRVSkill> UpgradeSkill;

	// Unit Upgrade
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		EGearUpgradePart GearUpgradePart = EGearUpgradePart::NONE;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 GearUpgradeIndex;


};
