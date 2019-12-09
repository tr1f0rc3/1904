// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "KRVPlayerState.h"
#include "Structs/KRVHeroSaveData.h"
#include "Structs/KRVPlayerStats.h"
#include "Structs/KRVPlayerResourceData.h"
#include "Structs/KRVItemStockData.h"
#include "Structs/KRVLevelSaveData.h"
#include "KRVInGamePlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnLevelUpDelegate, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGoldChangedDelegate, int32);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemStockChangedDelegate, int32, class AKRVItem*);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnExpChangeDelegate, int32, int32);

UCLASS()
class KRV_API AKRVInGamePlayerState : public AKRVPlayerState
{
	GENERATED_BODY()

public:
	AKRVInGamePlayerState();

public: // load / save data
	virtual bool ResetInGameSave() override;
	virtual bool LoadInGameSave() override;
	virtual bool SaveInGameSave() override;
	bool SaveDataDuringGame();

public: // Hero
	UFUNCTION(BlueprintCallable)
		int32 GetHeroLevel() const;

	UFUNCTION(BlueprintCallable)
		void GetHeroSaveData(FKRVHeroSaveData& OutHeroSaveData);

	UFUNCTION(BlueprintCallable)
		void GainExp(int32 ExpToGain);

	UFUNCTION(BlueprintCallable)
		void GetHeroSkillMasteryLevels(TArray<FKRVSkillMasterySaveData>& OutSkillMasterySaveData) const;

	UFUNCTION(BlueprintCallable)
		float GetCurrentExp() const;

	UFUNCTION(BlueprintCallable)
		float GetExpToLevelUp() const;

	// Hero object will call this fcn.
	// 영웅 오브젝트가 이 함수를 부름.
	void SetExpToLevelUp(int32 NewExp);

protected:
	void CheckExpReachedAndLevelUp();

public:
	FOnLevelUpDelegate OnLevelUpDelegate;
	FOnExpChangeDelegate OnExpChangeDelegate;

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Hero")
		int32 ExpToLevelUp;

	UPROPERTY(BlueprintReadOnly, Category = "Hero")
		FKRVHeroSaveData HeroSaveData;

public: // Towers
	UFUNCTION(BlueprintCallable)
		bool GetEquippedTowerNames(TArray<FName>& OutNames);

protected:
	UPROPERTY(BlueprintReadOnly)
		TArray<FName> EquippedTowerNames;

public: // Items
	UFUNCTION(BlueprintCallable)
		bool GetItemStockData(TArray<FKRVItemStockData>& OutItemStockData);

	UFUNCTION(BlueprintCallable)
		int32 GetRemainingItemStock(class AKRVItem* ItemToGet) const;

	UFUNCTION(BlueprintCallable)
		int32 UseItem(class AKRVItem* ItemToUse);

public:
	FOnItemStockChangedDelegate OnItemStockChangedDelegate;

protected:
	UPROPERTY(BlueprintReadOnly)
		TArray<FKRVItemStockData> ItemStockData;

public: // Rewards
	UFUNCTION(BlueprintCallable)
		void EarnRuby(int32 RubyToEarn);

protected:
	UPROPERTY(BlueprintReadOnly)
		FKRVPlayerResourceData ResourceData;

public: // Statistics
	UFUNCTION(BlueprintCallable)
		void GainKillRewards(int32 _Gold, int32 _Exp, int32 _Ruby = 0, int32 _Kills = 1);

	UFUNCTION(BlueprintCallable)
		void AddKills(int32 Kills = 1);

	UFUNCTION(BlueprintCallable)
		void EarnGold(int32 _Gold);

	UFUNCTION(BlueprintCallable)
		void AddSpentGold(int32 _Gold);

	UFUNCTION(BlueprintCallable)
		void AddRecruits(int32 _Recruits = 1);

protected:
	UPROPERTY(BlueprintReadOnly)
		FKRVPlayerStats PlayerStats;

public:
	UFUNCTION(BlueprintCallable)
		bool GetLevelSaveData(FKRVLevelSaveData& OutLevelSaveData);

	void SetClearData(int32 EarnedStar, float ClearTime);

protected:
	UPROPERTY(BlueprintReadOnly)
		FKRVLevelSaveData LevelSaveData;

};
