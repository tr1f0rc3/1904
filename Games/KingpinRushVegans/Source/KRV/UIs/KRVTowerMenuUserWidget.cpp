// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVTowerMenuUserWidget.h"
#include "SizeBox.h"
#include "TimerManager.h"

#include "KRV.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVTower.h"
#include "KRVTowerUpgradeSlotUserWidget.h"
#include "KRVIconUserWidget.h"

void UKRVTowerMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UKRVTowerMenuUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	MainSizeBox = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Main_1")));
	MainSizeBox->SetVisibility(ESlateVisibility::Hidden);

	TowerUpgradeSlots.Reserve(MaxTowerUpgradesToHave);
	TowerUpgradeSlots.Emplace(Cast<UKRVTowerUpgradeSlotUserWidget>(GetWidgetFromName(TEXT("UI_TowerUpgradeSlot_0"))));
	TowerUpgradeSlots.Emplace(Cast<UKRVTowerUpgradeSlotUserWidget>(GetWidgetFromName(TEXT("UI_TowerUpgradeSlot_1"))));
	TowerUpgradeSlots.Emplace(Cast<UKRVTowerUpgradeSlotUserWidget>(GetWidgetFromName(TEXT("UI_TowerUpgradeSlot_2"))));
	TowerUpgradeSlots.Emplace(Cast<UKRVTowerUpgradeSlotUserWidget>(GetWidgetFromName(TEXT("UI_TowerUpgradeSlot_3"))));
	TowerUpgradeSlots.Emplace(Cast<UKRVTowerUpgradeSlotUserWidget>(GetWidgetFromName(TEXT("UI_TowerUpgradeSlot_4"))));

	TowerSellingSlot = Cast<UKRVTowerUpgradeSlotUserWidget>(GetWidgetFromName(TEXT("UI_TowerSellingSlot_1")));

}

bool UKRVTowerMenuUserWidget::ShowThisWidget_Implementation(bool bIsTrue)
{
	ESlateVisibility CurrentVisbility = GetVisibility();
	if (bIsTrue && CurrentVisbility == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return true;
	}
	else if (!bIsTrue && CurrentVisbility == ESlateVisibility::SelfHitTestInvisible)
	{
		if (bIsProgressing)
		{
			bDoesNeedToHideThisWidgetOnUpgradeFinished = true;
		}
		else
		{
			return true;
		}
	}
	return false;

}

void UKRVTowerMenuUserWidget::ShowAllIcons(bool bIsTrue)
{
	for (UKRVTowerUpgradeSlotUserWidget* TowerUpgradeSlot : TowerUpgradeSlots)
	{
		if (!TowerUpgradeSlot)
		{
			KRVCHECK(TowerUpgradeSlot);
		}
		else if (TowerUpgradeSlot->IsBoundTo())
		{
			TowerUpgradeSlot->ShowThisWidget(bIsTrue);
		}
	}

	if (!TowerSellingSlot)
	{
		KRVCHECK(TowerSellingSlot);
	}
	else
	{
		TowerSellingSlot->ShowThisWidget(bIsTrue);
	}

}

void UKRVTowerMenuUserWidget::SetThisWidgetHiddenOnAnimFinished()
{
	SetVisibility(ESlateVisibility::Hidden);

}

bool UKRVTowerMenuUserWidget::BindToTower(AKRVTower* NewTower)
{
	if (!NewTower)
	{
		KRVCHECK(NewTower);
	}
	else
	{
		bool bBindResult = true;
		BoundTower = NewTower;

		for (int32 Index = 0; Index < MaxTowerUpgradesToHave; ++Index)
		{
			if (!TowerUpgradeSlots.IsValidIndex(Index))
			{
				bBindResult = false;
			}
			else if (TowerUpgradeSlots[Index]->BindToTowerUpgrade(Index, NewTower))
			{
				TowerUpgradeSlots[Index]->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			}
			else
			{
				TowerUpgradeSlots[Index]->SetVisibility(ESlateVisibility::Hidden);
			}
		}

		if (!TowerSellingSlot)
		{
			KRVCHECK(TowerSellingSlot);
			bBindResult = false;
		}
		else
		{
			bBindResult &= TowerSellingSlot->BindToTowerSelling(NewTower);
			TowerSellingSlot->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		}

		NewTower->OnSelectedDelegate.AddUObject(this, &UKRVTowerMenuUserWidget::OnSelected);
		NewTower->OnUpgradeBeginDelegate.AddUObject(this, &UKRVTowerMenuUserWidget::OnUpgradeBegin);
		NewTower->OnUpgradeFinishedDelegate.AddUObject(this, &UKRVTowerMenuUserWidget::OnUpgradeFinished);
		NewTower->OnSellingBeginDelegate.AddUObject(this, &UKRVTowerMenuUserWidget::OnSellingBegin);
		NewTower->OnSellingFinishedDelegate.AddUObject(this, &UKRVTowerMenuUserWidget::OnSellingFinished);

		return bBindResult;
	}
	return false;

}

void UKRVTowerMenuUserWidget::OnUpgradeBegin(UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, AKRVTower* NewTower)
{
	if (!NewTower || !NewTowerUpgradeDA)
	{
		KRVCHECK(NewTower);
		KRVCHECK(NewTowerUpgradeDA);
	}
	else if (BoundTower != NewTower)
	{
		KRVCHECK(BoundTower == NewTower);
	}
	else
	{ 
		// check any slot have the DA
		bool bDoesHaveTheTowerUpgradeDA = false;
		for (UKRVTowerUpgradeSlotUserWidget* TowerUpgradeSlot : TowerUpgradeSlots)
		{
			if (!TowerUpgradeSlot)
			{
				KRVCHECK(TowerUpgradeSlot);
			}
			else if (TowerUpgradeSlot->IsBoundTo() && TowerUpgradeSlot->GetTowerUpgradeDA() == NewTowerUpgradeDA)
			{
				bDoesHaveTheTowerUpgradeDA = true;
			}
		}

		if (bDoesHaveTheTowerUpgradeDA)
		{
			bIsProgressing = true;
			if (NewTowerUpgradeDA->TowerUpgradeType == ETowerUpgradeType::NewTower)
			{
				bDoesNeedToHideThisWidgetOnUpgradeFinished = true;
			}

			for (UKRVTowerUpgradeSlotUserWidget* TowerUpgradeSlot : TowerUpgradeSlots)
			{
				if (!TowerUpgradeSlot)
				{
					KRVCHECK(TowerUpgradeSlot);
				}
				else if (TowerUpgradeSlot->IsBoundTo())
				{
					if (TowerUpgradeSlot->GetTowerUpgradeDA() != NewTowerUpgradeDA)
					{
						TowerUpgradeSlot->ShowThisWidget(false);
					}
				}
			}

			if (TowerSellingSlot)
			{
				TowerSellingSlot->ShowThisWidget(false);
			}
		}
	}

}

void UKRVTowerMenuUserWidget::OnUpgradeFinished(UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, AKRVTower* NewTower)
{
	if (!NewTower || !NewTowerUpgradeDA)
	{
		KRVCHECK(NewTower);
		KRVCHECK(NewTowerUpgradeDA);
	}
	else if (BoundTower != NewTower)
	{
		KRVCHECK(BoundTower == NewTower);
	}
	else
	{
		if (TowerSellingSlot && TowerSellingSlot->IsBoundTo() &&
			NewTowerUpgradeDA->TowerUpgradeType != ETowerUpgradeType::NewTower)
		{
			TowerSellingSlot->UpdateSellingCost();
		}
		HideWidgetOrRestoreIconsAfterProgressFinished();
	}

}

void UKRVTowerMenuUserWidget::HideWidgetOrRestoreIconsAfterProgressFinished()
{
	bIsProgressing = false;

	if (GetWorld())
	{
		if (bDoesNeedToHideThisWidgetOnUpgradeFinished)
		{
			bDoesNeedToHideThisWidgetOnUpgradeFinished = false;
			GetWorld()->GetTimerManager().SetTimer(HideThisWidgetTimer, this,
				&UKRVTowerMenuUserWidget::HideAfterUpgradeFinished, DelayHidingThisWidgetWhenUpgradeFinished, false);
		}
		else
		{
			GetWorld()->GetTimerManager().SetTimer(RestoreIconsTimer, this,
				&UKRVTowerMenuUserWidget::RestoreIconsWhenUpgradeFinished, RestoreIconsDelayWhenUpgradeFinished, false);
		}
	}

}

void UKRVTowerMenuUserWidget::OnSellingBegin(AKRVTower* NewTower)
{
	if (!NewTower)
	{
		KRVCHECK(NewTower);
	}
	else if (BoundTower != NewTower || !NewTower->IsSellable())
	{
		KRVCHECK(BoundTower == NewTower);
		KRVCHECK(NewTower->IsSellable());
	}
	else
	{
		bIsProgressing = true;
		bDoesNeedToHideThisWidgetOnUpgradeFinished = true;

		for (UKRVTowerUpgradeSlotUserWidget* TowerUpgradeSlot : TowerUpgradeSlots)
		{
			if (!TowerUpgradeSlot)
			{
				KRVCHECK(TowerUpgradeSlot);
			}
			else if (TowerUpgradeSlot->IsBoundTo())
			{
				TowerUpgradeSlot->ShowThisWidget(false);
			}
		}
	}

}

void UKRVTowerMenuUserWidget::OnSellingFinished(AKRVTower* NewTower)
{
	if (!NewTower || !TowerSellingSlot)
	{
		KRVCHECK(NewTower);
		KRVCHECK(TowerSellingSlot);
	}
	else if (BoundTower != NewTower)
	{
		KRVCHECK(BoundTower == NewTower);
	}
	else if (TowerSellingSlot->IsBoundTo())
	{
		HideWidgetOrRestoreIconsAfterProgressFinished();
	}

}

void UKRVTowerMenuUserWidget::OnSelected(bool bIsSelected, AKRVCombatUnit* NewTower)
{
	if (!NewTower || !TowerSellingSlot)
	{
		KRVCHECK(NewTower);
		KRVCHECK(TowerSellingSlot);
	}
	else if (BoundTower != NewTower)
	{
		KRVCHECK(BoundTower == NewTower);
	}
	else
	{
		// Reset ConsecutiveClicks
		for (UKRVTowerUpgradeSlotUserWidget* TowerUpgradeSlot : TowerUpgradeSlots)
		{
			if (TowerUpgradeSlot)
			{
				UKRVIconUserWidget* TowerUpgradeIconWidget = TowerUpgradeSlot->GetTowerUpgradeIconWidget();
				if (TowerUpgradeIconWidget)
				{
					TowerUpgradeIconWidget->ResetConsecutiveClicks();
				}
			}
		}
		if (TowerSellingSlot)
		{
			UKRVIconUserWidget* TowerSellingIconWidget = TowerSellingSlot->GetTowerUpgradeIconWidget();
			if (TowerSellingIconWidget)
			{
				TowerSellingIconWidget->ResetConsecutiveClicks();
			}
		}

		ShowThisWidget(bIsSelected);
	}

}

void UKRVTowerMenuUserWidget::RestoreIconsWhenUpgradeFinished()
{
	ShowAllIcons(true);

}

void UKRVTowerMenuUserWidget::HideAfterUpgradeFinished()
{
	ShowThisWidget(false);
	ShowAllIcons(true);

}
