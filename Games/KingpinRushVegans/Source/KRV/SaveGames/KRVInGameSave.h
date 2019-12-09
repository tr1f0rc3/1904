// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Structs/KRVLevelSaveData.h"
#include "Structs/KRVPlayerResourceData.h"
#include "Structs/KRVCommanderSaveData.h"
#include "Structs/KRVHeroSaveData.h"
#include "Structs/KRVItemStockData.h"
#include "Structs/KRVPlayerStats.h"
#include "KRVInGameSave.generated.h"

UCLASS()
class KRV_API UKRVInGameSave : public USaveGame
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
		bool LoadInGameSaveFromSaveGame(const FString& NewSaveSlotString, const FString& LevelString);

	UFUNCTION(BlueprintCallable)
		bool SaveInGameSaveToSaveGame();

public:
	UPROPERTY(BlueprintReadWrite)
		FString SaveSlotString;

	UPROPERTY(BlueprintReadWrite)
		FKRVHeroSaveData HeroSaveData;

	UPROPERTY(BlueprintReadWrite)
		FKRVLevelSaveData LevelSaveData;

	UPROPERTY(BlueprintReadWrite)
		FKRVPlayerResourceData ResourceData;

	UPROPERTY(BlueprintReadWrite)
		TArray<FKRVItemStockData> ItemStockData;

	UPROPERTY(BlueprintReadWrite)
		FKRVCommanderSaveData CommanderData;

	UPROPERTY(BlueprintReadWrite)
		TArray<FName> EquippedTowerNames = {"LIZARD_1", "DEER_1", "BADGER_1", "OWL_1"};

	UPROPERTY(BlueprintReadWrite)
		FKRVPlayerStats PlayerStats;

	
};
