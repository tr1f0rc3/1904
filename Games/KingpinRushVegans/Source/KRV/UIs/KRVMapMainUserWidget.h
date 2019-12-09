// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "Structs/KRVLevelSaveData.h"
#include "KRVMapMainUserWidget.generated.h"

UCLASS()
class KRV_API UKRVMapMainUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public: // Show & hide widget
	UFUNCTION(BlueprintNativeEvent)
		void ShowThisWidget();
	void ShowThisWidget_Implementation();

protected: // Load
	void OnGameMenuWidgetClassAsyncLoaded();
	void OnHeroMenuWidgetClassAsyncLoaded();
	void OnItemMenuWidgetClassAsyncLoaded();
	void OnTowerMenuWidgetClassAsyncLoaded();

protected:
	FSoftClassPath GameMenuWidgetClassPath = "/Game/Resources/UI/UI_InGame_Menu.UI_InGame_Menu_C";
	FSoftClassPath HeroMenuWidgetClassPath;
	FSoftClassPath ItemMenuWidgetClassPath;
	FSoftClassPath TowerMenuWidgetClassPath;

public: // Player state
	void BindToPlayerState(class AKRVMapPlayerState* NewPlayerState);

protected: // Game Menu
	UFUNCTION()
		void OnGameMenuIconClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* MenuIconWidget;

	FName GameMenuTitle = "Option menu";
	FName GameMenuText = "Shows several game options.";

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVInGameMenuUserWidget> GameMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameMenuUserWidget* GameMenuWidget;

protected: // Hero menu
	UFUNCTION()
		void OnHeroMenuIconClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* HeroIconWidget;

	FName HeroMenuTitle = "Hero menu";
	FName HeroMenuText = "Shows the menu to pick a hero and learn skills.";

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVMapHeroMenuUserWidget> HeroMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVMapHeroMenuUserWidget* HeroMenuWidget;

protected: // Item menu
	UFUNCTION()
		void OnItemMenuIconClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ItemIconWidget;

	FName ItemMenuTitle = "Item menu";
	FName ItemMenuText = "Shows the menu to purchase and equip items.";

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVInGameMenuUserWidget> ItemMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVMapItemMenuUserWidget* ItemMenuWidget;

protected: // Tower menu
	UFUNCTION()
		void OnTowerMenuIconClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* TowerIconWidget;

	FName TowerMenuTitle = "Tower menu";
	FName TowerMenuText = "Shows the menu to pick towers to build.";

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVMapTowerMenuUserWidget> TowerMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVMapTowerMenuUserWidget* TowerMenuWidget;

};
