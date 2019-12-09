// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVMapItemMenuUserWidget.h"
#include "TextBlock.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"

#include "Structs/KRVItemStockData.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "SaveGames/KRVSaveGame.h"
#include "Controllers/KRVMapPlayerController.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "KRVGameInstance.h"

#include "KRVIconUserWidget.h"
#include "KRVMapItemSlotUserWidget.h"

void UKRVMapItemMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_1"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_2"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_3"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_4"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_5"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_6"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_7"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_8"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_9"))));
	ItemSlots.Emplace(Cast<UKRVMapItemSlotUserWidget>(GetWidgetFromName(TEXT("UI_ItemSlot_10"))));

	EquippedItemIcons.Reserve(MaxItemsToEquip);
	EquippedItemIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ItemIcon_1"))));
	EquippedItemIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ItemIcon_2"))));
	EquippedItemIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ItemIcon_3"))));

	ConfirmIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ConfirmIcon_1")));
	if (ConfirmIconWidget)
	{
		ConfirmIconWidget->SetCustomMenuIcon(EIconTextureVariation::Confirm);
		ConfirmIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapItemMenuUserWidget::OnConfirmIconClicked);
	}

	CancleIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_CancleIcon_1")));
	if (CancleIconWidget)
	{
		CancleIconWidget->SetCustomMenuIcon(EIconTextureVariation::Cancle);
		CancleIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapItemMenuUserWidget::OnCancleIconClicked);
	}

	RubyTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Ruby_1")));

}

void UKRVMapItemMenuUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

}

void UKRVMapItemMenuUserWidget::HideThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVMapItemMenuUserWidget::OnConfirmIconClicked()
{
	if (GetWorld())
	{
		AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
		if (KRVMapPlayerController)
		{
			KRVMapPlayerController->SaveSaveGame();
		}
	}
	HideThisWidget();

}

void UKRVMapItemMenuUserWidget::OnCancleIconClicked()
{
	HideThisWidget();

}

void UKRVMapItemMenuUserWidget::BindToPlayerState(class AKRVMapPlayerState* PlayerStateToBind)
{
	if (!PlayerStateToBind)
	{
		KRVCHECK(PlayerStateToBind);
	}
	else if (GetWorld())
	{
		UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(GetWorld());
		UKRVSaveGame* KRVSaveGame = PlayerStateToBind->GetSaveGame();
		if (!KRVSaveGame || !KRVGameInstance)
		{
			KRVCHECK(KRVSaveGame);
			KRVCHECK(KRVGameInstance);
		}
		else
		{
			// Bind delegate
			PlayerStateToBind->OnRubyChangedDelegate.AddUObject(this, &UKRVMapItemMenuUserWidget::OnRubyChanged);
			PlayerStateToBind->OnItemEquippedDelegate.AddUObject(this, &UKRVMapItemMenuUserWidget::OnItemEquipped);
			PlayerStateToBind->OnItemUnequippedDelegate.AddUObject(this, &UKRVMapItemMenuUserWidget::OnItemUnequipped);

			// Bind item data
			for (int32 Index = 0; ItemSlots.IsValidIndex(Index) &&
				KRVSaveGame->ItemStockData.IsValidIndex(Index); ++Index)
			{
				const FKRVItemStockData& ItemStockDataToBind = KRVSaveGame->ItemStockData[Index];
				const FName& ItemNameToBind = ItemStockDataToBind.ItemName;
				UKRVMapItemSlotUserWidget* ItemSlotToBind = ItemSlots[Index];
				UKRVItemDataAsset* ItemDAToBind = KRVGameInstance->GetItemDA(ItemStockDataToBind.ItemName);
				if (!ItemDAToBind)
				{
					KRVCHECK(ItemDAToBind);
				}
				else
				{
					// save data to array
					int32 ItemDAEmplaceIndex = ItemDAArray.Emplace(ItemDAToBind);
					int32 ItemStockDataEmplaceIndex = ItemStockDataArray.Emplace(ItemStockDataToBind);
					KRVCHECK(ItemDAEmplaceIndex == ItemStockDataEmplaceIndex);
					ItemIndexMap.Emplace(ItemNameToBind, ItemDAEmplaceIndex);

					// binding
					ItemSlotToBind->BindToItemInMap(ItemDAToBind, ItemStockDataToBind);
					UKRVIconUserWidget* IconWidgetToBind = ItemSlotToBind->GetItemIconWidget();


					if (!IconWidgetToBind)
					{
						KRVCHECK(IconWidgetToBind);
					}
					else if (PlayerStateToBind->IsEquippedItem(ItemNameToBind))
					{
						IconWidgetToBind->OnItemEquipped(ItemNameToBind);
						OnItemEquipped(ItemNameToBind);
					}
				}
			}
		}
	}

}

void UKRVMapItemMenuUserWidget::OnItemEquipped(const FName& EquippedItem)
{
	if (EquipCount > MaxItemsToEquip)
	{
		KRVCHECK(EquipCount <= MaxItemsToEquip);
	}
	else if (EquipCount == MaxItemsToEquip)
	{
		KRVLOG(Log, TEXT("Item inventory is full."))
	}
	else
	{
		int32 IconIndexToEquip = -1;
		for (int32 Index = 0; Index < MaxItemsToEquip; ++Index)
		{
			if (!EquippedItemIcons.IsValidIndex(Index))
			{
				KRVCHECK(EquippedItemIcons.IsValidIndex(Index));
			}
			else if (!EquippedItemIcons[Index]->IsBound())
			{
				IconIndexToEquip = Index;
				break;
			}
		}

		if (!EquippedItemIcons.IsValidIndex(IconIndexToEquip))
		{
			KRVCHECK(EquippedItemIcons.IsValidIndex(IconIndexToEquip));
		}
		else
		{
			UKRVIconUserWidget* IconToEquip = EquippedItemIcons[IconIndexToEquip];
			int32* EquippedItemIndexPtr = ItemIndexMap.Find(EquippedItem);
			if (!EquippedItemIndexPtr)
			{
				KRVCHECK(EquippedItemIndexPtr);
			}
			else
			{
				int32 EquippedItemIndex = *EquippedItemIndexPtr;
				if (!ItemDAArray.IsValidIndex(EquippedItemIndex) || !ItemStockDataArray.IsValidIndex(EquippedItemIndex))
				{
					KRVCHECK(ItemDAArray.IsValidIndex(EquippedItemIndex));
					KRVCHECK(ItemStockDataArray.IsValidIndex(EquippedItemIndex));
				}
				else
				{
					IconToEquip->BindToItemInMap
					(ItemDAArray[EquippedItemIndex], ItemStockDataArray[EquippedItemIndex], true);
				}
			}
		}
	}

}

void UKRVMapItemMenuUserWidget::OnItemUnequipped(const FName& EquippedItem)
{
	if (EquipCount <= 0)
	{
		KRVCHECK(EquipCount > 0);
	}
	else
	{
		for (UKRVIconUserWidget* IconToUnequip : EquippedItemIcons)
		{
			if (IconToUnequip->IsBound())
			{
				FName ItemNameToCheck;
				IconToUnequip->GetItemName(ItemNameToCheck);
				if (ItemNameToCheck == EquippedItem)
				{
					IconToUnequip->SetItemVacant();
				}
			}
		}
	}

}

void UKRVMapItemMenuUserWidget::OnRubyChanged(int32 NewRuby)
{
	if (RubyTextBlock)
	{
		RubyTextBlock->SetText(FText::FromString(FString::FromInt(NewRuby)));
	}

}
