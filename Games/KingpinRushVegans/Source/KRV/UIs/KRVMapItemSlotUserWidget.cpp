// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVMapItemSlotUserWidget.h"
#include "Border.h"
#include "TextBlock.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "KRVIconUserWidget.h"
#include "PlayerStates/KRVMapPlayerState.h"

void UKRVMapItemSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UKRVMapItemSlotUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ItemIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_Item_1")));
	StockBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BD_Stock_1")));
	StockTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Stock_1")));
	CostBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BD_Cost_1")));
	CostTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Cost_1")));
	PurchaseIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_Purchase_1")));

}

UKRVIconUserWidget* UKRVMapItemSlotUserWidget::GetItemIconWidget() const
{
	return ItemIconWidget;

}

bool UKRVMapItemSlotUserWidget::BindToItemInMap(UKRVItemDataAsset* ItemDAToBind, const FKRVItemStockData& ItemStockDataToBind)
{
	bIsBound = false;
	if (!ItemDAToBind || !ItemIconWidget)
	{
		KRVCHECK(ItemDAToBind);
		KRVCHECK(ItemIconWidget);
	}
	else
	{
		bIsBound = true;
		BoundItemName = ItemStockDataToBind.ItemName;
		AKRVMapPlayerState* KRVMapPlayerState = nullptr;
		if (GetWorld())
		{
			KRVMapPlayerState = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerState(GetWorld());
		}

		bIsBound &= ItemIconWidget->BindToItemInMap(ItemDAToBind, ItemStockDataToBind);

		if (StockBorder)
		{
			if (ItemStockDataToBind.bIsUnlocked)
			{
				MaxItemStocks = ItemDAToBind->MaxStocks;
				OnItemPurchased(ItemStockDataToBind.Stock, ItemStockDataToBind.ItemName);
				if (KRVMapPlayerState)
				{
					KRVMapPlayerState->OnItemPurchasedDelegate.AddUObject(this, &UKRVMapItemSlotUserWidget::OnItemPurchased);
				}
				else
				{
					bIsBound = false;
				}
			}
			else
			{
				StockBorder->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		else
		{
			bIsBound = false;
		}

		if (PurchaseIconWidget)
		{
			if (ItemStockDataToBind.bIsUnlocked)
			{
				PurchaseIconWidget->BindToItemPurchase(ItemDAToBind, ItemStockDataToBind);
				PurchaseIconWidget->SetVisibility(ESlateVisibility::Visible);

			}
			else
			{
				PurchaseIconWidget->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (CostBorder && CostTextBlock)
		{
			CostTextBlock->SetText(FText::FromString(FString::FromInt(ItemDAToBind->Price)));
			StockBorder->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			bIsBound = false;
		}

		return bIsBound;
	}
	return false;

}

void UKRVMapItemSlotUserWidget::OnItemPurchased(int32 NewStock, const FName& ItemPurchased)
{
	if (BoundItemName == ItemPurchased)
	{
		if (StockBorder && StockTextBlock && MaxItemStocks != -1)
		{
			FString StockString = FString::FromInt(NewStock) + FString(TEXT("/")) + FString::FromInt(MaxItemStocks);
			StockTextBlock->SetText(FText::FromString(StockString));
			StockBorder->SetVisibility(ESlateVisibility::Visible);
		}
		else
		{
			StockBorder->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}
