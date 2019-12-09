// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVCombatUnitDataAsset.h"
#include "Structs/KRVCharacterSpawnData.h"
#include "KRVTowerDataAsset.generated.h"

UCLASS()
class KRV_API UKRVTowerDataAsset : public UKRVCombatUnitDataAsset
{
	GENERATED_BODY()

public:
	// TowerInfo
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		int32 TowerLevel;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		bool bIsSellable = true;

	// Mesh
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSoftObjectPtr<class UStaticMesh> PlatformMesh;

	// Upgrades
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TArray<TSoftObjectPtr<class UKRVTowerUpgradeDataAsset>> TowerUpgrades;

	// Only for barracks
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		FKRVCharacterSpawnData CharacterSpawnData;

};
