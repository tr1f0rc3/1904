// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/KRVPlayerController.h"
#include "Structs/KRVLevelSaveData.h"
#include "KRVMapPlayerController.generated.h"

UCLASS()
class KRV_API AKRVMapPlayerController : public AKRVPlayerController
{
	GENERATED_BODY()

public:
	AKRVMapPlayerController();

protected:
	void BeginPlay();

public:
	class AKRVMapPlayerState* GetKRVMapPlayerState();
	class UKRVSaveGame* GetSaveGame();
	bool SaveSaveGame();

protected:
	UPROPERTY()
		class AKRVMapPlayerState* KRVMapPlayerState;

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVMapMainUserWidget> MapMainWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVMapMainUserWidget* MapMainWidget;

public: // Level widget
	UFUNCTION()
		void OnBattlefieldIconClicked(class UKRVLevelDataAsset* NewLevelDA, const FKRVLevelSaveData& NewLevelSaveDA);

protected:
	bool BindWidgetToLevel(class UKRVLevelDataAsset* NewLevelDA, const FKRVLevelSaveData& NewLevelSaveDA);
	void ShowBattlefieldWidgets();

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVLevelInfoUserWidget> LevelInfoWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVLevelInfoUserWidget* LevelInfoWidget;


public: // Level
	bool OpenInGameLevelByDA(const class UKRVLevelDataAsset* LevelDAToOpen, const FKRVLevelSaveData& LevelSaveDataToOpen);
	bool OpenInGameLevelByString(const FString& LevelStringToOpen, const FKRVLevelSaveData& LevelSaveDataToOpen);

protected:
	void RequestToShowUI();
	void ShowUIAfterFadeIn();
	void OpenInGameLevelAfterFadeOut();

protected:
	FString ReservedLevelStringToOpen;

public: // Hero
	bool PickHero(const FName& NewHeroName);
	bool GivePointToHeroSkill(class UKRVDataAsset* NewDA, const FName& NewHeroName);
	bool ResetHeroSkillPoints(const FName& NewHeroName);

public: // EquipItem
	bool EquipItem(const FName& ItemToEquip);
	bool UnequipItem(const FName& ItemToUnequip);
	bool BuyItem(const class UKRVItemDataAsset* ItemDAToBuy, int32 AmountToBuy = 1);

public: // Tower
	bool EquipTower(const FName& TowerToEquip);
	bool UnequipTower(const FName& TowerToUnequip);

};
