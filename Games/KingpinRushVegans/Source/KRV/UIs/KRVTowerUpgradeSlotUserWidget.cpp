// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVTowerUpgradeSlotUserWidget.h"
#include "Border.h"
#include "TextBlock.h"

#include "KRV.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"
#include "Pawns/KRVTower.h"

#include "KRVIconUserWidget.h"


void UKRVTowerUpgradeSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UKRVTowerUpgradeSlotUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	TowerUpgradeIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerUpgradeIcon_1")));
	LevelBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BD_Level_1")));
	LevelTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Level_1")));
	CostBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BD_Cost_1")));
	CostTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Cost_1")));

}

bool UKRVTowerUpgradeSlotUserWidget::IsBoundTo() const
{
	return bIsBindInitialized;

}

UKRVIconUserWidget* UKRVTowerUpgradeSlotUserWidget::GetTowerUpgradeIconWidget() const
{
	return TowerUpgradeIconWidget;

}

bool UKRVTowerUpgradeSlotUserWidget::ShowThisWidget_Implementation(bool bIsTrue)
{
	ESlateVisibility CurrentVisbility = GetVisibility();
	if (bIsTrue && CurrentVisbility == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return true;
	}
	else if (!bIsTrue && CurrentVisbility == ESlateVisibility::SelfHitTestInvisible)
	{
		return true;
	}
	else
	{
		return false;
	}


}


void UKRVTowerUpgradeSlotUserWidget::SetThisWidgetHiddenOnAnimFinished()
{
	SetVisibility(ESlateVisibility::Hidden);

}

UKRVTowerUpgradeDataAsset* UKRVTowerUpgradeSlotUserWidget::GetTowerUpgradeDA() const
{
	if (IconType != EIconType::TowerUpgrade)
	{
		KRVCHECK(IconType == EIconType::TowerUpgrade);
		return nullptr;
	}
	else
	{
		return BoundTowerUpgradeDA;
	}

}

bool UKRVTowerUpgradeSlotUserWidget::BindToTowerUpgrade(int32 UpgradeIndex, AKRVTower* NewTower)
{
	if (!TowerUpgradeIconWidget || !TowerUpgradeIconWidget->BindToTowerUpgrade(UpgradeIndex, NewTower))
	{
		KRVCHECK(TowerUpgradeIconWidget);
	}
	else
	{
		bool bBindResult = true;
		if (!LevelTextBlock || !LevelBorder || !CostTextBlock || !CostBorder)
		{
			KRVCHECK(LevelBorder);
			KRVCHECK(CostBorder);
			KRVCHECK(LevelTextBlock);
			KRVCHECK(CostTextBlock);
			bBindResult = false;
		}

		UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA = NewTower->GetTowerUpgradeDA(UpgradeIndex);
		int32 CurrentTowerUpgradeLevel = NewTower->GetTowerUpgradeLevel(UpgradeIndex);

		IconType = EIconType::TowerUpgrade;
		BoundTower = NewTower;
		BoundTowerUpgradeDAIndex = UpgradeIndex;
		BoundTowerUpgradeDA = NewTowerUpgradeDA;

		if (CostBorder)
		{
			if (CurrentTowerUpgradeLevel == NewTowerUpgradeDA->MaxUpgradeLevel)
			{
				CostBorder->SetVisibility(ESlateVisibility::Hidden);
			}
			else
			{
				CostBorder->SetVisibility(ESlateVisibility::Visible);
				if (CostTextBlock)
				{
					CostTextBlock->SetText
					(FText::FromString(FString::FromInt(NewTower->GetActualTowerUpgradeCost(UpgradeIndex))));
				}
			}
		}

		switch (NewTowerUpgradeDA->TowerUpgradeType)
		{
		case ETowerUpgradeType::NewTower:
		{
			if (LevelBorder)
			{
				LevelBorder->SetVisibility(ESlateVisibility::Hidden);
			}
			break;
		}
		default:
		{
			if (LevelBorder)
			{
				LevelBorder->SetVisibility(ESlateVisibility::Visible);
			}
			break;
		}
		}

		OnUpgradeFinished(NewTowerUpgradeDA, NewTower);
		NewTower->OnUpgradeBeginDelegate.Remove(BoundUpgradeBeginDelegateHandle);
		BoundUpgradeBeginDelegateHandle = NewTower->
			OnUpgradeBeginDelegate.AddUObject(this, &UKRVTowerUpgradeSlotUserWidget::OnUpgradeBegin);
		NewTower->OnUpgradeBeginDelegate.Remove(BoundUpgradeFinishDelegateHandle);
		BoundUpgradeFinishDelegateHandle = NewTower->
			OnUpgradeFinishedDelegate.AddUObject(this, &UKRVTowerUpgradeSlotUserWidget::OnUpgradeFinished);

		bIsBindInitialized = bBindResult;
		return bBindResult;
	}
	return false;

}

bool UKRVTowerUpgradeSlotUserWidget::BindToTowerSelling(class AKRVTower* NewTower)
{
	if (!TowerUpgradeIconWidget || !TowerUpgradeIconWidget->BindToTowerSelling(NewTower))
	{
		KRVCHECK(TowerUpgradeIconWidget);
	}
	else
	{
		bool bBindResult = true;
		if (!LevelTextBlock || !LevelBorder || !CostTextBlock || !CostBorder)
		{
			KRVCHECK(LevelBorder);
			KRVCHECK(CostBorder);
			KRVCHECK(LevelTextBlock);
			KRVCHECK(CostTextBlock);
			bBindResult = false;
		}

		IconType = EIconType::TowerSelling;
		BoundTower = NewTower;
		BoundTowerUpgradeDAIndex = -1;
		BoundTowerUpgradeDA = nullptr;

		if (LevelTextBlock && LevelBorder)
		{
			LevelBorder->SetVisibility(ESlateVisibility::Hidden);
		}

		if (CostTextBlock && CostBorder)
		{
			if (NewTower->IsSellable())
			{
				CostBorder->SetVisibility(ESlateVisibility::Visible);
				UpdateSellingCost();
				NewTower->OnSellingBeginDelegate.AddUObject(this, &UKRVTowerUpgradeSlotUserWidget::OnSellingBegin);
				NewTower->OnSellingFinishedDelegate.AddUObject(this, &UKRVTowerUpgradeSlotUserWidget::OnSellingFinished);
			}
			else
			{
				CostBorder->SetVisibility(ESlateVisibility::Hidden);
			}
		}
		bIsBindInitialized = bBindResult;
		return bBindResult;
	}
	return false;

}


void UKRVTowerUpgradeSlotUserWidget::OnUpgradeBegin(class UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, AKRVTower* NewTower)
{
	if (!NewTower || !NewTowerUpgradeDA || !CostBorder || IconType != EIconType::TowerUpgrade)
	{
		KRVCHECK(NewTower);
		KRVCHECK(NewTowerUpgradeDA);
		KRVCHECK(CostBorder);
		KRVCHECK(IconType == EIconType::TowerUpgrade)
	}
	else if (BoundTower != NewTower || BoundTowerUpgradeDA != NewTowerUpgradeDA)
	{
		KRVCHECK(BoundTower == NewTower);
		KRVCHECK(BoundTowerUpgradeDA == NewTowerUpgradeDA);
	}
	else
	{
		CostBorder->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UKRVTowerUpgradeSlotUserWidget::OnUpgradeFinished(UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA, AKRVTower* NewTower)
{
	if (!NewTower || !NewTowerUpgradeDA || !CostBorder || IconType != EIconType::TowerUpgrade)
	{
		KRVCHECK(NewTower);
		KRVCHECK(NewTowerUpgradeDA);
		KRVCHECK(CostBorder);
		KRVCHECK(IconType == EIconType::TowerUpgrade)
	}
	else if (BoundTower != NewTower || BoundTowerUpgradeDA != NewTowerUpgradeDA)
	{
		KRVCHECK(BoundTower == NewTower);
		KRVCHECK(BoundTowerUpgradeDA == NewTowerUpgradeDA);
	}
	else
	{
		if (bIsBindInitialized)
		{
			// ReplaceWithNewTowerUpgrade() will be called in blueprint after UserWidgetAnim. If the updated is replaced with next upgrade, the text is no need to be updated.
			// ��� ����Ʈ���� ���� �ִϸ��̼��� ����� �� ReplaceWithNewTowerUpgrade()�� �Ҹ�. ���׷��̵尡 �� ���׷��̵�� ��ü�Ǵ� ��� �ؽ�Ʈ�� ������Ʈ �� �ʿ䰡 ����.
			if (!NewTowerUpgradeDA->NextTowerUpgradeDA)
			{
				FString NewLevelString = FString::FromInt(NewTowerUpgradeDA->UpgradeLevelTo) + FString(TEXT("/")) + FString::FromInt(NewTowerUpgradeDA->MaxUpgradeLevel);
				LevelTextBlock->SetText(FText::FromString(NewLevelString));
			}
			PlayOnUpgradeFinishedEffect();
		}
		else
		{
			int32 NewUpgradeLevel = NewTower->GetTowerUpgradeLevel(NewTower->GetIndexByTowerUpgradeDA(NewTowerUpgradeDA));
			FString NewLevelString = FString::FromInt(NewUpgradeLevel) + FString(TEXT("/")) + FString::FromInt(NewTowerUpgradeDA->MaxUpgradeLevel);
			LevelTextBlock->SetText(FText::FromString(NewLevelString));
		}
	}

}

void UKRVTowerUpgradeSlotUserWidget::OnSellingBegin(AKRVTower* NewTower)
{
	if (!NewTower || !CostBorder || IconType != EIconType::TowerSelling)
	{
		KRVCHECK(CostBorder);
		KRVCHECK(NewTower);
		KRVCHECK(IconType == EIconType::TowerSelling);
	}
	else if (BoundTower != NewTower)
	{
		KRVCHECK(BoundTower == NewTower);
	}
	else
	{
		CostBorder->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UKRVTowerUpgradeSlotUserWidget::OnSellingFinished(AKRVTower* NewTower)
{
	// not implemented

}

void UKRVTowerUpgradeSlotUserWidget::ReplaceWithNewTowerUpgrade()
{
	if (!TowerUpgradeIconWidget)
	{
		KRVCHECK(TowerUpgradeIconWidget);
	}
	else
	{
		if (BoundTower && BoundTowerUpgradeDA)
		{
			if (BoundTowerUpgradeDA->UpgradeLevelTo < BoundTowerUpgradeDA->MaxUpgradeLevel)
			{
				bIsBindInitialized = false;
				BindToTowerUpgrade(BoundTowerUpgradeDAIndex, BoundTower);
			}
		}
	}

}

bool UKRVTowerUpgradeSlotUserWidget::UpdateSellingCost()
{
	if (!CostTextBlock || !BoundTower)
	{
		KRVCHECK(CostTextBlock);
		KRVCHECK(BoundTower);
	}
	else
	{
		CostTextBlock->SetText(FText::FromString(FString::FromInt(BoundTower->GetSellingCost())));
		return true;
	}
	return false;

}

