// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "KRVInGameMainUserWidget.generated.h"

UCLASS()
class KRV_API UKRVInGameMainUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public: // Show & hide widget
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool ShowThisWidget(bool bIsTrue);
	bool ShowThisWidget_Implementation(bool bIsTrue);

protected:
	UFUNCTION(BlueprintCallable)
		void SetThisWidgetHiddenOnAnimFinished();

protected: // Load
	void OnGameMenuWidgetClassAsyncLoaded();
	void OnUnitInfoWidgetClassAsyncLoaded();

protected:
	FSoftClassPath GameMenuWidgetClassPath = "/Game/Resources/UI/UI_InGame_Menu.UI_InGame_Menu_C";
	FSoftClassPath UnitInfoWidgetClassPath = "/Game/Resources/UI/UI_InGame_UnitInfo.UI_InGame_UnitInfo_C";

public: // Controller;
	bool BindPCTextBlockToPC(class AKRVInGamePlayerController* NewController);

protected:
	UFUNCTION()
		void OnTargetingBegin(ETargetingType NewTargetingType);

	UFUNCTION()
		void OnTargetingEnd();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* PCTextBlock;

	FString OnUnitTargetingBeginString = FString(TEXT("Select target unit."));
	FString OnLocationTargetingBeginString = FString(TEXT("Select target location."));

public: // Hero & Commander
	bool BindToHero(class AKRVHero* NewHero);
	bool BindToCommander(class AKRVCombatUnit* NewCommander);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameMainHeroUserWidget* HeroWidgets;

	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameCommanderUserWidget* CommanderWidgets;

	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameCommanderUserWidget* ItemWidgets;

protected: // HUD
	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameHUDUserWidget* HUDWidgets;

protected: // Log
	UPROPERTY(BlueprintReadOnly)
		class UKRVLogBoxUserWidget* LogBoxWidget;

protected: // Menu
	UFUNCTION()
		void OnGameMenuIconClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* MenuIconWidget;

	FName MenuIconTitle = "Option menu";
	FName MenuIconText = "Pauses the game and show option menu.";

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVInGameMenuUserWidget> GameMenuWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameMenuUserWidget* GameMenuWidget;

protected:
	bool BindUnitInfoWidgetToPC(class AKRVInGamePlayerController* NewController);

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVUnitInfoUserWidget> UnitInfoWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UKRVUnitInfoUserWidget* UnitInfoWidget;

};
