// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Structs/KRVItemStockData.h"
#include "KRVMapItemMenuUserWidget.generated.h"

UCLASS()
class KRV_API UKRVMapItemMenuUserWidget : public UUserWidget
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
		TArray<class UKRVMapItemSlotUserWidget*> ItemSlots;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UKRVIconUserWidget*> EquippedItemIcons;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ConfirmIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* CancleIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* RubyTextBlock;

public:
	void BindToPlayerState(class AKRVMapPlayerState* PlayerStateToBind);

protected:
	UFUNCTION()
		void OnItemEquipped(const FName& EquippedItem);

	UFUNCTION()
		void OnItemUnequipped(const FName& EquippedItem);

	UFUNCTION()
		void OnRubyChanged(int32 NewRuby);

protected:
	UPROPERTY()
		TArray<class UKRVItemDataAsset*> ItemDAArray;

	UPROPERTY()
		TArray<FKRVItemStockData> ItemStockDataArray;

	TMap<FName, int32> ItemIndexMap;
	int32 EquipCount = 0;

};
