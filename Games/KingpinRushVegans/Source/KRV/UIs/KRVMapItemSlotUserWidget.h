// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "Structs/KRVItemStockData.h"
#include "KRVMapItemSlotUserWidget.generated.h"

/**
 *
 */
UCLASS()
class KRV_API UKRVMapItemSlotUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

public: // Widgets
	UFUNCTION(BlueprintCallable)
		class UKRVIconUserWidget* GetItemIconWidget() const;

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ItemIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UBorder* StockBorder;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* StockTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UBorder* CostBorder;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* CostTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* PurchaseIconWidget;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsBound;

public: // Item selection in map
	bool BindToItemInMap(class UKRVItemDataAsset* ItemDAToBind, const FKRVItemStockData& ItemStockDataToBind);

protected:
	UFUNCTION()
		void OnItemPurchased(int32 NewStock, const FName& ItemPurchased);

protected:
	FName BoundItemName;
	int32 MaxItemStocks = -1;

};
