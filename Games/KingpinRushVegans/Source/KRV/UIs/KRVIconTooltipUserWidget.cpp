// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVIconTooltipUserWidget.h"
#include "WidgetLayoutLibrary.h"
#include "Components/CanvasPanelSlot.h"
#include "SizeBox.h"
#include "TextBlock.h"

#include "KRV.h"
#include "DataAssets/KRVDataAsset.h"
#include "DataAssets/KRVHeroDataAsset.h"
#include "DataAssets/KRVTowerDataAsset.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"
#include "Actors/KRVSkill.h"
#include "Pawns/KRVTower.h"
#include "Actors/KRVGateway.h"

void UKRVIconTooltipUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	MainSizeBox = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Main_1")));
	TitleTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Title_1")));
	TitleTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	TextTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Text_1")));
	TextTextBlock->SetVisibility(ESlateVisibility::Collapsed);
	WarningTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Warning_1")));
	WarningTextBlock->SetVisibility(ESlateVisibility::Collapsed);

	SetVisibility(ESlateVisibility::Hidden);

}

bool UKRVIconTooltipUserWidget::ShowThisWidget_Implementation(bool bIsTrue)
{
	ESlateVisibility CurrentVisbility = GetVisibility();
	if (bIsTrue && (CurrentVisbility == ESlateVisibility::Hidden))
	{
		if (IsAllTextsEmpty())
		{
			return false;
		}
		else
		{
			SetVisibility(ESlateVisibility::HitTestInvisible);
			return true;
		}
	}
	else if (!bIsTrue && (CurrentVisbility == ESlateVisibility::HitTestInvisible))
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool UKRVIconTooltipUserWidget::IsBoundTo() const
{
	return bIsBound;

}

bool UKRVIconTooltipUserWidget::BindToSkill(AKRVSkill* NewSkill)
{
	if (!NewSkill)
	{
		KRVCHECK(NewSkill);
	}
	else if (!NewSkill->GetDataAsset_Implementation())
	{
		KRVCHECK(NewSkill->GetDataAsset_Implementation());
	}
	else
	{
		ResetTexts();
		bool bBindingResult = true;
		FText NewText;
		FString NewString;
		int32 SkillMasteryLevel = NewSkill->GetMasteryLevel();

		// Title
		NewText = NewSkill->GetDataAsset_Implementation()->ClassFText;
		if (SkillMasteryLevel > 0)
		{
			NewString = NewText.ToString() + FString(TEXT(" ")) + FString::FromInt(SkillMasteryLevel);
			NewText = FText::FromString(NewString);
		}
		bBindingResult &= SetTitleText(NewText);

		// Text
		if (NewSkill->GetDescriptionWithVaraibles(NewText))
		{
			bBindingResult &= SetTextText(NewText);
		}
		else
		{
			bBindingResult = false;
		}

		// Warning
		if (SkillMasteryLevel <= 0)
		{
			NewText = FText::FromName(NotLearnedSkillText);
			bBindingResult &= SetWarningText(NewText);
		}

		bIsBound = bBindingResult;
		return bBindingResult;
	}
	return false;

}

bool UKRVIconTooltipUserWidget::BindToDataAsset(class UKRVDataAsset* NewDA)
{
	if (!NewDA)
	{
		KRVCHECK(NewDA);
		return false;
	}
	else
	{
		ResetTexts();
		bIsBound = SetTexts(NewDA->ClassFText, NewDA->Description);
		return bIsBound;
	}

}

bool UKRVIconTooltipUserWidget::BindToHero(class UKRVDataAsset* NewDA)
{
	if (!NewDA)
	{
		KRVCHECK(NewDA);
	}
	else
	{
		ResetTexts();
		bIsBound = SetTexts(NewDA->ClassFText, FText::FromName(HeroText));
		return bIsBound;
	}
	return false;

}

bool UKRVIconTooltipUserWidget::BindToTowerSelling(AKRVTower* NewTower)
{
	if (!NewTower)
	{
		KRVCHECK(NewTower);
	}
	else
	{
		ResetTexts();
		bool bBindingResult = true;

		bBindingResult &= SetTexts(FText::GetEmpty(), FText::FromName(TowerSellingText));
		if (!NewTower->IsSellable())
		{
			bBindingResult &= SetWarningText(FText::FromName(UnableToSellingText));
		}

		bIsBound = bBindingResult;
		return bBindingResult;
	}
	return false;

}

bool UKRVIconTooltipUserWidget::BindToTowerUpgrade(int32 NewUpgradeIndex, AKRVTower* NewTower)
{
	if (!NewTower)
	{
		KRVCHECK(NewTower);
	}
	else
	{
		UKRVTowerUpgradeDataAsset* TowerUpgradeDA = NewTower->GetTowerUpgradeDA(NewUpgradeIndex);
		if (!TowerUpgradeDA)
		{
			KRVCHECK(TowerUpgradeDA);
		}
		else
		{
			bool bBindingResult = true;
			bBindingResult &= BindToDataAsset(Cast<UKRVDataAsset>(TowerUpgradeDA));
			if (NewTower->GetTowerUpgradeLevel(NewUpgradeIndex) >= TowerUpgradeDA->MaxUpgradeLevel)
			{
				bBindingResult &= SetWarningText(FText::FromName(MaxTowerUpgradeText));
			}
			bIsBound = bBindingResult;
			return bBindingResult;
		}
	}
	return false;

}

bool UKRVIconTooltipUserWidget::BindtoGateway(class AKRVGateway* NewGateway)
{
	if (!NewGateway)
	{
		KRVCHECK(NewGateway);
	}
	else
	{
		
		FText TextTextToSet;
		FText WarningTextToSet;
		if (NewGateway->GetNextWaveTooltipText(TextTextToSet))
		{
			if (NewGateway->IsFlyingUnitIncomingInNextWave())
			{
				WarningTextToSet = FText::FromName(FlyingUnitWarningText);
			}
			bIsBound = SetTexts(FText::FromName(NextWaveIncomingText), TextTextToSet), FlyingUnitWarningText;
			return bIsBound;
		}
	}
	return false;

}

bool UKRVIconTooltipUserWidget::BindToHeroInMap(class UKRVHeroDataAsset* HeroDAToBind, bool bIsUnlocked)
{
	bIsBound = false;
	if (!HeroDAToBind)
	{
		KRVCHECK(HeroDAToBind);
	}
	else
	{
		bIsBound = true;
		bIsBound &= SetTexts(HeroDAToBind->ClassFText, HeroDAToBind->Description);
		if (!bIsUnlocked)
		{
			bIsBound &= SetWarningText(FText::FromString(UnlockedText.ToString()));
		}
		return bIsBound;
	}
	return false;

}

bool UKRVIconTooltipUserWidget::BindToItemInMap(class UKRVItemDataAsset* ItemDAToBind, bool bIsUnlocked)
{
	bIsBound = false;
	if (!ItemDAToBind)
	{
		KRVCHECK(ItemDAToBind);
	}
	else
	{
		bIsBound = true;
		bIsBound &= SetTexts(ItemDAToBind->ClassFText, ItemDAToBind->Description);
		if (!bIsUnlocked)
		{
			bIsBound &= SetWarningText(FText::FromString(UnlockedText.ToString()));
		}
		return bIsBound;
	}
	return false;


}

bool UKRVIconTooltipUserWidget::BindToTowerInMap(class UKRVTowerDataAsset* TowerDAToBind, bool bIsUnlocked)
{
	bIsBound = false;
	if (!TowerDAToBind)
	{
		KRVCHECK(TowerDAToBind);
	}
	else
	{
		bIsBound = true;
		bIsBound &= SetTexts(TowerDAToBind->ClassFText, TowerDAToBind->Description);
		if (!bIsUnlocked)
		{
			bIsBound &= SetWarningText(FText::FromString(UnlockedText.ToString()));
		}
		return bIsBound;
	}
	return false;

}

bool UKRVIconTooltipUserWidget::SetTexts(const FText& NewTitle, const FText& NewText, const FText& NewRequirement)
{
	bool bSetResult = false;
	bSetResult |= SetTitleText(NewTitle);
	bSetResult |= SetTextText(NewText);
	bSetResult |= SetWarningText(NewRequirement);
	return bSetResult;

}

bool UKRVIconTooltipUserWidget::SetTitleText(const FText& NewText)
{
	if (!TitleTextBlock)
	{
		KRVCHECK(TitleTextBlock);
	}
	else if (NewText.IsEmptyOrWhitespace())
	{
		TitleTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return true;
	}
	else
	{
		TitleTextBlock->SetText(NewText);
		TitleTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
		SetSizeBoxAuto();
		return true;
	}
	return false;

}

void UKRVIconTooltipUserWidget::SetSizeBoxAuto()
{
	if (MainSizeBox)
	{
		UWidgetLayoutLibrary::SlotAsCanvasSlot(MainSizeBox)->SetAutoSize(true);
	}

}

bool UKRVIconTooltipUserWidget::SetTextText(const FText& NewText)
{
	if (!TextTextBlock)
	{
		KRVCHECK(TextTextBlock);
	}
	else if (NewText.IsEmptyOrWhitespace())
	{
		TextTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return true;
	}
	else
	{
		TextTextBlock->SetText(NewText);
		TextTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
		SetSizeBoxAuto();
		return true;
	}
	return false;

}

bool UKRVIconTooltipUserWidget::SetWarningText(const FText& NewText)
{
	if (!WarningTextBlock)
	{
		KRVCHECK(WarningTextBlock);
	}
	else if (NewText.IsEmptyOrWhitespace())
	{
		WarningTextBlock->SetVisibility(ESlateVisibility::Collapsed);
		return true;
	}
	else
	{
		WarningTextBlock->SetText(NewText);
		WarningTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
		SetSizeBoxAuto();
		return true;
	}
	return false;

}

void UKRVIconTooltipUserWidget::HideThisWidget()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVIconTooltipUserWidget::ResetTexts()
{
	SetTexts(FText::GetEmpty(), FText::GetEmpty(), FText::GetEmpty());
	bIsBound = false;

}

bool UKRVIconTooltipUserWidget::IsAllTextsEmpty()
{
	if (!TitleTextBlock || !TextTextBlock || !WarningTextBlock)
	{
		return false;
	}
	else
	{
		return TitleTextBlock->GetVisibility() == ESlateVisibility::Collapsed &&
			TextTextBlock->GetVisibility() == ESlateVisibility::Collapsed &&
			WarningTextBlock->GetVisibility() == ESlateVisibility::Collapsed;
	}

}
