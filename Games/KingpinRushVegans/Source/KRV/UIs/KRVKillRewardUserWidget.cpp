// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVKillRewardUserWidget.h"
#include "HorizontalBox.h"
#include "TextBlock.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "GameModes/KRVInGameGameMode.h"

void UKRVKillRewardUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	ExpHorizontalBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_Exp_1")));
	ExpTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Exp_1")));
	GoldHorizontalBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_Gold_1")));
	GoldTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Gold_1")));
	RubyHorizontalBox = Cast<UHorizontalBox>(GetWidgetFromName(TEXT("HB_Ruby_1")));
	RubyTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Ruby_1")));

	SetVisibility(ESlateVisibility::Hidden);

}

bool UKRVKillRewardUserWidget::BindToCombatUnit(class AKRVCombatUnit* NewUnit)
{
	if (!NewUnit)
	{
		KRVCHECK(NewUnit);
	}
	else
	{
		NewUnit->OnDieWithRewardDelegate.AddUObject(this, &UKRVKillRewardUserWidget::OnUnitDieWithReward);
		return true;
	}
	return false;

}

void UKRVKillRewardUserWidget::HideThisWidget()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVKillRewardUserWidget::OnUnitDieWithReward(int32 NewGold, int32 NewExp, int32 NewRuby, AKRVCombatUnit* NewUnit)
{
	if (GetWorld())
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		if (KRVInGameGameMode && KRVInGameGameMode->IsKiiledByPlayer(Cast<AKRVCharacter>(NewUnit)))
		{
			SetTextAndVisibilityOfTheBoxes(NewExp, NewGold, NewRuby);
			PopUpThisWidget();
		}
	}

}

void UKRVKillRewardUserWidget::SetTextAndVisibilityOfTheBoxes(int32 NewExperience, int32 NewGold, int32 NewRuby)
{
	FText TextToSet;

	if (ExpHorizontalBox)
	{
		if (NewExperience == 0)
		{
			ExpHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (ExpTextBlock)
		{
			IntToPlusMinusText(TextToSet, NewExperience);
			ExpTextBlock->SetText(TextToSet);
			ExpHorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}

	if (GoldHorizontalBox)
	{
		if (NewGold == 0)
		{
			GoldHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (GoldTextBlock)
		{
			IntToPlusMinusText(TextToSet, NewGold);
			GoldTextBlock->SetText(TextToSet);
			GoldHorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}

	if (RubyHorizontalBox)
	{
		if (NewRuby == 0)
		{
			RubyHorizontalBox->SetVisibility(ESlateVisibility::Collapsed);
		}
		else if (RubyTextBlock)
		{
			IntToPlusMinusText(TextToSet, NewRuby);
			RubyTextBlock->SetText(TextToSet);
			RubyHorizontalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
	}

}

void UKRVKillRewardUserWidget::IntToPlusMinusText(FText& OutText, int32 NewValue)
{
	FString StringToSet;
	if (NewValue > 0)
	{
		StringToSet += FString(TEXT("+"));
	}
	else if (NewValue < 0)
	{
		StringToSet += FString(TEXT("-"));
	}

	StringToSet += FString::FromInt(FMath::Abs(NewValue));
	OutText = FText::FromString(StringToSet);

}

void UKRVKillRewardUserWidget::PopUpThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

}
