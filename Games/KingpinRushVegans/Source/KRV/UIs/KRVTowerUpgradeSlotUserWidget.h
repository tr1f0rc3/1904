// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"

#include "KRVTowerUpgradeSlotUserWidget.generated.h"

/**
 *
 */
UCLASS()
class KRV_API UKRVTowerUpgradeSlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

public: // Widgets
	UFUNCTION(BlueprintCallable)
		bool IsBoundTo() const;

	UFUNCTION(BlueprintCallable)
		class UKRVIconUserWidget* GetTowerUpgradeIconWidget() const;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool ShowThisWidget(bool bIsTrue);
	bool ShowThisWidget_Implementation(bool bIsTrue);

protected:
	UFUNCTION(BlueprintCallable)
		void SetThisWidgetHiddenOnAnimFinished();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* TowerUpgradeIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UBorder* LevelBorder;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* LevelTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UBorder* CostBorder;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* CostTextBlock;

public: // Towers
	UFUNCTION(BlueprintCallable)
		class UKRVTowerUpgradeDataAsset* GetTowerUpgradeDA() const;

	UFUNCTION(BlueprintCallable)
		bool UpdateSellingCost();

	bool BindToTowerUpgrade(int32 UpgradeIndex, class AKRVTower* NewTower);
	bool BindToTowerSelling(class AKRVTower* NewTower);

protected:
	UFUNCTION()
		void OnUpgradeBegin(class UKRVTowerUpgradeDataAsset* NewDA, class AKRVTower* NewTower);

	UFUNCTION()
		void OnUpgradeFinished(class UKRVTowerUpgradeDataAsset* NewDA, class AKRVTower* NewTower);

	UFUNCTION()
		void OnSellingBegin(class AKRVTower* NewTower);

	UFUNCTION()
		void OnSellingFinished(class AKRVTower* NewTower);

	UFUNCTION(BlueprintImplementableEvent)
		void PlayOnUpgradeFinishedEffect();

	UFUNCTION(BlueprintCallable)
		void ReplaceWithNewTowerUpgrade();

	
protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		EIconType IconType;

	UPROPERTY(Transient, BlueprintReadOnly)
		class AKRVTower* BoundTower;

	UPROPERTY(Transient, BlueprintReadOnly)
		class UKRVTowerUpgradeDataAsset* BoundTowerUpgradeDA;

	UPROPERTY(Transient, BlueprintReadOnly)
		int32 BoundTowerUpgradeDAIndex;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsBindInitialized;

	FDelegateHandle BoundUpgradeFinishDelegateHandle;
	FDelegateHandle BoundUpgradeBeginDelegateHandle;

};
