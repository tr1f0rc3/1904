// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "Structs/KRVLevelStarQualification.h"
#include "KRVLevelDataAsset.generated.h"

UCLASS()
class KRV_API UKRVLevelDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Overview")
		TSoftObjectPtr<class UTexture2D> OverviewTexture;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
		int32 GivenHearts = 20;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Unlock")
		TArray<FKRVLevelStarQualification> LevelStarQualifications;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
		TArray<class UKRVLevelDataAsset*> LevelsToUnlockWhenClear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
		TArray<class UKRVTowerUpgradeDataAsset*> TowersToUnlockWhenClear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
		TArray<class UKRVHeroDataAsset*> HeroesToUnlockWhenClear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
		TArray<class UKRVItemDataAsset*> ItemsToUnlockWhenClear;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
		float RubyToEarn = 50;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Reward")
		int32 CommanderPointsToEarn = 1;
	
};
