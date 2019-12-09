// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"
#include "Structs/KRVHeroSaveData.h"
#include "Structs/KRVLevelSaveData.h"
#include "Structs/KRVItemStockData.h"
#include "Structs/KRVTowerSaveData.h"
#include "Structs/KRVCommanderSaveData.h"
#include "Structs/KRVPlayerResourceData.h"
#include "Structs/KRVPlayerStats.h"
#include "KRVSaveGame.generated.h"

UCLASS()
class KRV_API UKRVSaveGame : public USaveGame
{
	GENERATED_BODY()

public:
	UKRVSaveGame();

	UPROPERTY(BlueprintReadWrite, Category = "ID")
		FString SaveSlotString;

public: // Gameplay variables
	bool SpendRuby(int32 RubyToSpend);

public:
	UPROPERTY(BlueprintReadWrite)
		FKRVPlayerResourceData ResourceData;

	UPROPERTY(BlueprintReadWrite)
		FKRVCommanderSaveData CommanderSaveData;

	UPROPERTY(BlueprintReadWrite)
		FKRVPlayerStats PlayerStats;

public: // Hero
	bool ChooseHeroByName(const FName& HeroToChoose);
	bool IsChosenHero(const FName& HeroToCheck);
	bool GetChosenHeroData(FKRVHeroSaveData& OutHeroSaveData);
	bool GetHeroSaveData(FKRVHeroSaveData& OutHeroSaveData, const FName& HeroToGet);
	void SetHeroSaveData(const FKRVHeroSaveData& NewHeroSaveData);

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FKRVHeroSaveData> HeroSaveData;

protected:
	UPROPERTY(BlueprintReadOnly)
		FName ChosenHeroName;

	UPROPERTY(BlueprintReadOnly)
		int32 ChosenHeroIndex;

public: // Item
	void GetEquippedItemStockData(TArray<FKRVItemStockData>& OutItemStockData);
	bool EquipItem(const FName& ItemNameToEquip);
	bool UnequipItem(const FName& ItemNameToUnequip);
	bool IsEquippedItem(const FName& ItemNameToCheck) const;
	void SetItemStockData(const TArray<FKRVItemStockData>& NewItemStockData);
	void SetItemStockData(const FKRVItemStockData& NewItemStockData);
	bool GetItemStockData(FKRVItemStockData& OutItemStockData, const FName& ItemNameToGet);

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FKRVItemStockData> ItemStockData;

protected:
	UPROPERTY(BlueprintReadOnly)
		TMap<FName, int32> EquippedItemMap;

public: // Level
	bool GetLevelSaveData(FKRVLevelSaveData& OutLevelSaveData, const FString& NewLevelString);
	void SetLevelSaveData(const FKRVLevelSaveData& NewLevelSaveData);

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FKRVLevelSaveData> LevelSaveData;

public: // Tower
	bool GetTowerSaveData(FKRVTowerSaveData& OutTowerSaveData, const FName& TowerToGet);
	void SetTowerSaveData(const FKRVTowerSaveData& NewTowerSaveData);
	void GetEquippedTowerNames(TArray<FName>& OutTowerSaveData) const;
	bool EquipTower(const FName& TowerToEquip);
	bool UnequipTower(const FName& TowerToUnequip);
	bool IsEquippedTower(const FName& TowerToCheck) const;

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FKRVTowerSaveData> TowerSaveData;

protected:
	UPROPERTY(BlueprintReadOnly)
		TMap<FName, int32> EquippedTowerMap;

};
