// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVMapTowerMenuUserWidget.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Structs/KRVTowerSaveData.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "Controllers/KRVMapPlayerController.h"
#include "KRVGameInstance.h"
#include "SaveGames/KRVSaveGame.h"
#include "KRVIconUserWidget.h"
#include "PlayerStates/KRVMapPlayerState.h"

void UKRVMapTowerMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_1"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_2"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_3"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_4"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_5"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_6"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_7"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_8"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_9"))));
	TowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_10"))));

	EquippedTowerIcons.Reserve(MaxTowersToEquip);
	EquippedTowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_EquippedTowerIcon_1"))));
	EquippedTowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_EquippedTowerIcon_2"))));
	EquippedTowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_EquippedTowerIcon_3"))));
	EquippedTowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_EquippedTowerIcon_4"))));
	EquippedTowerIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_EquippedTowerIcon_5"))));

	ConfirmIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ConfirmIcon_1")));
	if (ConfirmIconWidget)
	{
		ConfirmIconWidget->SetCustomMenuIcon(EIconTextureVariation::Confirm);
		ConfirmIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapTowerMenuUserWidget::OnConfirmIconClicked);
	}

	CancleIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_CancleIcon_1")));
	if (CancleIconWidget)
	{
		CancleIconWidget->SetCustomMenuIcon(EIconTextureVariation::Cancle);
		CancleIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapTowerMenuUserWidget::OnCancleIconClicked);
	}

}

void UKRVMapTowerMenuUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

}

void UKRVMapTowerMenuUserWidget::HideThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVMapTowerMenuUserWidget::OnConfirmIconClicked()
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

void UKRVMapTowerMenuUserWidget::OnCancleIconClicked()
{
	HideThisWidget();

}

void UKRVMapTowerMenuUserWidget::OnTowerEquipped(const FName& EquippedTower)
{
	if (EquipCount > MaxTowersToEquip)
	{
		KRVCHECK(EquipCount <= MaxTowersToEquip);
	}
	else if (EquipCount == MaxTowersToEquip)
	{
		KRVLOG(Log, TEXT("Tower inventory is full."))
	}
	else
	{
		int32 IconIndexToEquip = -1;
		for (int32 Index = 0; Index < MaxTowersToEquip; ++Index)
		{
			if (!EquippedTowerIcons.IsValidIndex(Index))
			{
				KRVCHECK(EquippedTowerIcons.IsValidIndex(Index));
			}
			else if (!EquippedTowerIcons[Index]->IsBound())
			{
				IconIndexToEquip = Index;
				break;
			}
		}

		if (!EquippedTowerIcons.IsValidIndex(IconIndexToEquip))
		{
			KRVCHECK(EquippedTowerIcons.IsValidIndex(IconIndexToEquip));
		}
		else
		{
			UKRVIconUserWidget* IconToEquip = EquippedTowerIcons[IconIndexToEquip];
			int32* EquippedTowerIndexPtr = TowerIndexMap.Find(EquippedTower);
			if (!EquippedTowerIndexPtr)
			{
				KRVCHECK(EquippedTowerIndexPtr);
			}
			else
			{
				int32 EquippedTowerIndex = *EquippedTowerIndexPtr;
				if (!TowerDAArray.IsValidIndex(EquippedTowerIndex) || !TowerSaveDataArray.IsValidIndex(EquippedTowerIndex))
				{
					KRVCHECK(TowerDAArray.IsValidIndex(EquippedTowerIndex));
					KRVCHECK(TowerSaveDataArray.IsValidIndex(EquippedTowerIndex));
				}
				else
				{
					IconToEquip->BindToTowerInMap
					(TowerDAArray[EquippedTowerIndex], TowerSaveDataArray[EquippedTowerIndex], true);
				}
			}
		}
	}

}

void UKRVMapTowerMenuUserWidget::OnTowerUnequipped(const FName& EquippedTower)
{
	if (EquipCount <= 0)
	{
		KRVCHECK(EquipCount > 0);
	}
	else
	{
		for (UKRVIconUserWidget* IconToUnequip : EquippedTowerIcons)
		{
			if (IconToUnequip->IsBound())
			{
				FName ItemNameToCheck;
				IconToUnequip->GetItemName(ItemNameToCheck);
				if (ItemNameToCheck == EquippedTower)
				{
					IconToUnequip->SetItemVacant();
				}
			}
		}
	}

}

void UKRVMapTowerMenuUserWidget::BindToPlayerState(AKRVMapPlayerState* PlayerStateToBind)
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
			PlayerStateToBind->OnTowerEquippedDelegate.AddUObject(this, &UKRVMapTowerMenuUserWidget::OnTowerEquipped);
			PlayerStateToBind->OnTowerUnequippedDelegate.AddUObject(this, &UKRVMapTowerMenuUserWidget::OnTowerUnequipped);

			for (int32 Index = 0; TowerIcons.IsValidIndex(Index) &&
				KRVSaveGame->TowerSaveData.IsValidIndex(Index); ++Index)
			{
				const FKRVTowerSaveData& TowerSaveDataToBind = KRVSaveGame->TowerSaveData[Index];
				const FName& TowerNameToBind = TowerSaveDataToBind.TowerName;
				UKRVIconUserWidget* TowerIconToBind = TowerIcons[Index];
				UKRVTowerDataAsset* TowerDAToBind = KRVGameInstance->GetTowerDA(TowerSaveDataToBind.TowerName);
				if (!TowerDAToBind)
				{
					KRVCHECK(TowerDAToBind);
				}
				else
				{
					// save data to array
					int32 TowerDAEmplaceIndex = TowerDAArray.Emplace(TowerDAToBind);
					int32 TowerSaveDataEmplaceIndex = TowerSaveDataArray.Emplace(TowerSaveDataToBind);
					KRVCHECK(TowerDAEmplaceIndex == TowerSaveDataEmplaceIndex);
					TowerIndexMap.Emplace(TowerNameToBind, TowerDAEmplaceIndex);

					// Bind tower data
					TowerIconToBind->BindToTowerInMap(TowerDAToBind, TowerSaveDataToBind);
					if (PlayerStateToBind->IsEquippedItem(TowerNameToBind))
					{
						TowerIconToBind->OnItemEquipped(TowerNameToBind);
						OnTowerEquipped(TowerNameToBind);
					}
				}
			}
		}
	}

}

