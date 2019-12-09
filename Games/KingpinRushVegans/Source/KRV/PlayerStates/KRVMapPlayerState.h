// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PlayerStates/KRVPlayerState.h"
#include "KRVMapPlayerState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnHeroPickedDelegate, const FName&);
DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnPointGivenToHeroSkillDelegate, int32, class UKRVDataAsset*, const FName&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnRemainingSkillPointsChangedDelegate, int32, const FName&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemEquippedDelegate, const FName&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnItemUnequippedDelegate, const FName&);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnItemPurchasedDelegate, int32, const FName&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRubyChangedDelegate, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTowerEquippedDelegate, const FName&);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnTowerUnequippedDelegate, const FName&);

UCLASS()
class KRV_API AKRVMapPlayerState : public AKRVPlayerState
{
	GENERATED_BODY()

public:
	AKRVMapPlayerState();

public:
	bool ResetSaveGame();
	bool LoadSaveGame();
	bool SaveSaveGame();
	class UKRVSaveGame* GetSaveGame() const;
	bool ApplyInGameSaveToSaveGame();
	bool UpdateInGameSaveFromSaveGame(const FString& LevelStringToGet);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVSaveGame* LoadedSaveGame;

protected: // Level
	UFUNCTION()
		void OnRecentlyUnlockedLevelClicked(const FString& ClickedLevel);

	bool UnlockRewardsAndSaveGame(const class UKRVLevelDataAsset* NewLevelDA);

public: // Hero
	UFUNCTION()
		void OnRecentlyUpdatedHeroClicked(const FName& ClickedHero);

	bool PickHero(const FName& HeroNameToPick);
	bool IsPickedHero(const FName& HeroToCheck);
	bool GivePointToHeroSkill(class UKRVDataAsset* NewDA, const FName& NewHeroName);
	bool ResetHeroSkillPoints(const FName& NewHeroName);
	int32 GetPointOfHeroSkill(class UKRVDataAsset* NewDA, const FName& NewHeroName);

public:
	FOnHeroPickedDelegate OnHeroPickedDelegate;
	FOnPointGivenToHeroSkillDelegate OnPointGivenToHeroSkillDelegate;
	FOnRemainingSkillPointsChangedDelegate OnRemainingSkillPointsChangedDelegate;

public: // Item
	UFUNCTION()
		void OnRecentlyUnlockedItemClicked(const FName& ClickedItem);

	bool EquipItem(const FName& ItemToEquip);
	bool UnequipItem(const FName& ItemToUnequip);
	bool IsEquippedItem(const FName& ItemToCheck) const;
	bool BuyItem(const class UKRVItemDataAsset* ItemDAToBuy, int32 AmountToBuy = 1);

public:
	FOnItemEquippedDelegate OnItemEquippedDelegate;
	FOnItemUnequippedDelegate OnItemUnequippedDelegate;
	FOnItemPurchasedDelegate OnItemPurchasedDelegate;
	FOnRubyChangedDelegate OnRubyChangedDelegate;

public: // Tower
	UFUNCTION()
		void OnRecentlyUnlockedTowerClicked(const FName& ClickedTower);

	bool EquipTower(const FName& TowerToEquip);
	bool UnequipTower(const FName& TowerToUnequip);
	bool IsEquippedTower(const FName& TowerToCheck);

public:
	FOnTowerEquippedDelegate OnTowerEquippedDelegate;
	FOnTowerUnequippedDelegate OnTowerUnequippedDelegate;

};
