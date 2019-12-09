// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/KRVTowerSaveData.h"
#include "KRVMapTowerMenuUserWidget.generated.h"

UCLASS()
class KRV_API UKRVMapTowerMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ShowThisWidget();
	void ShowThisWidget_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void HideThisWidget();
	void HideThisWidget_Implementation();

	UFUNCTION()
		void OnConfirmIconClicked();

	UFUNCTION()
		void OnCancleIconClicked();

protected:
	UPROPERTY(BlueprintReadOnly)
		TArray<class UKRVIconUserWidget*> TowerIcons;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UKRVIconUserWidget*> EquippedTowerIcons;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ConfirmIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* CancleIconWidget;

public:
	void BindToPlayerState(class AKRVMapPlayerState* PlayerStateToBind);

protected:
	UFUNCTION()
		void OnTowerEquipped(const FName& EquippedTower);

	UFUNCTION()
		void OnTowerUnequipped(const FName& EquippedTower);

protected:
	UPROPERTY()
		TArray<class UKRVTowerDataAsset*> TowerDAArray;

	UPROPERTY()
		TArray<FKRVTowerSaveData> TowerSaveDataArray;

	TMap<FName, int32> TowerIndexMap;
	int32 EquipCount = 0;

};
