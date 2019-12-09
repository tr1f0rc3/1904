// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "KRVTowerMenuUserWidget.generated.h"

UCLASS()
class KRV_API UKRVTowerMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

public: // Widgets
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool ShowThisWidget(bool bIsTrue);
	bool ShowThisWidget_Implementation(bool bIsTrue);

protected:
	UFUNCTION(BlueprintCallable)
		void ShowAllIcons(bool bIsTrue);

	UFUNCTION(BlueprintCallable)
		void SetThisWidgetHiddenOnAnimFinished();

protected:
	UPROPERTY(BlueprintReadOnly)
		class USizeBox* MainSizeBox;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UKRVTowerUpgradeSlotUserWidget*> TowerUpgradeSlots;

	UPROPERTY(BlueprintReadOnly)
		class UKRVTowerUpgradeSlotUserWidget* TowerSellingSlot;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsProgressing;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsWidgetShown;

public: // Tower
	bool BindToTower(class AKRVTower* NewTower);

protected:
	UFUNCTION(BlueprintCallable)
		void RestoreIconsWhenUpgradeFinished();

	UFUNCTION(BlueprintCallable)
		void HideAfterUpgradeFinished();

	UFUNCTION()
		void OnUpgradeBegin(class UKRVTowerUpgradeDataAsset* NewDA, class AKRVTower* NewTower);

	UFUNCTION()
		void OnUpgradeFinished(class UKRVTowerUpgradeDataAsset* NewDA, class AKRVTower* NewTower);

	void HideWidgetOrRestoreIconsAfterProgressFinished();

	UFUNCTION()
		void OnSellingBegin(class AKRVTower* NewTower);

	UFUNCTION()
		void OnSellingFinished(class AKRVTower* NewTower);

	UFUNCTION()
		void OnSelected(bool bIsSelected, class AKRVCombatUnit* NewTower);

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		class AKRVTower* BoundTower;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		float RestoreIconsDelayWhenUpgradeFinished = 0.5f;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite)
		float DelayHidingThisWidgetWhenUpgradeFinished = 0.9f;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bDoesNeedToHideThisWidgetOnUpgradeFinished;

	FTimerHandle RestoreIconsTimer;
	FTimerHandle HideThisWidgetTimer;

};
