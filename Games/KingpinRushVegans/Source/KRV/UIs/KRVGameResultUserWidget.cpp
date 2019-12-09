// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVGameResultUserWidget.h"
#include "VerticalBox.h"
#include "SizeBox.h"
#include "TextBlock.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "KRVGameState.h"
#include "PlayerStates/KRVInGamePlayerState.h"

#include "UIs/KRVInGameMenuUserWidget.h"

void UKRVGameResultUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::Hidden);

	VictoryVerticalBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("VB_Victory_1")));
	StarSizeBox_1 = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Star_1")));
	StarSizeBox_2 = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Star_2")));
	StarSizeBox_3 = Cast<USizeBox>(GetWidgetFromName(TEXT("SB_Star_3")));
	DefeatVerticalBox = Cast<UVerticalBox>(GetWidgetFromName(TEXT("VB_Defeat_1")));
	ClearTimeTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_ClearTime_1")));
	RubyTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Ruby_1")));
	MenuWidgets = Cast<UKRVInGameMenuUserWidget>(GetWidgetFromName(TEXT("UI_GameEndMenu_1")));

}

void UKRVGameResultUserWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (bIsRubyCounting && RubyTextBlock)
	{
		RubyToShow = FMath::FInterpTo(RubyToShow, float(EarnedRuby), InDeltaTime, 3.f);
		RubyTextBlock->SetText(FText::FromString(FString::FromInt(FMath::RoundToInt(RubyToShow))));
		if (FMath::RoundToInt(RubyToShow) >= EarnedRuby)
		{
			bIsRubyCounting = false;
			ShowMenuWidget();
		}
	}

}

bool UKRVGameResultUserWidget::BindToGameState()
{
	if (GetWorld())
	{
		AKRVGameState* KRVGameState = UKRVBlueprintFunctionLibrary::GetKRVGameState(GetWorld());
		if (KRVGameState)
		{
			EarnedRuby = KRVGameState->GetEarnedRuby();
			if (RubyTextBlock)
			{
				RubyTextBlock->SetText(FText::FromString(FString::FromInt(0)));
			}

			if (ClearTimeTextBlock)
			{
				int32 SecondsAfterFirstWaveCalling = FMath::FloorToInt(KRVGameState->GetSecondsAfterFirstWaveCalling());
				FString StringToShow;
				UKRVBlueprintFunctionLibrary::GetStringFromSeconds(StringToShow, SecondsAfterFirstWaveCalling);
				ClearTimeTextBlock->SetText(FText::FromString(StringToShow));
			}

			GameStateEnum = KRVGameState->GetGameStateEnum();
			if (VictoryVerticalBox && DefeatVerticalBox)
			{
				switch (GameStateEnum)
				{
				case EGameState::Defeated:
				{
					VictoryVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
					DefeatVerticalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
					break;
				}
				case EGameState::Cleared:
				{
					if (StarSizeBox_1 && StarSizeBox_2 && StarSizeBox_3)
					{
						EarnedStars = KRVGameState->GetEarnedStars();
					}
					VictoryVerticalBox->SetVisibility(ESlateVisibility::HitTestInvisible);
					DefeatVerticalBox->SetVisibility(ESlateVisibility::Collapsed);
					break;
				}
				default:
					break;
				}
			}
			return true;
		}
	}
	return false;

}

void UKRVGameResultUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

	if (StarSizeBox_1)
	{
		StarSizeBox_1->SetVisibility(ESlateVisibility::Hidden);
	}

	if (StarSizeBox_2)
	{
		StarSizeBox_2->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (StarSizeBox_3)
	{
		StarSizeBox_3->SetVisibility(ESlateVisibility::Collapsed);
	}

	if (MenuWidgets)
	{
		MenuWidgets->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UKRVGameResultUserWidget::ShowMenuWidget_Implementation()
{
	if (!MenuWidgets)
	{
		KRVCHECK(MenuWidgets);
	}
	else
	{
		MenuWidgets->SetVisibility(ESlateVisibility::Visible);
	}

}

bool UKRVGameResultUserWidget::IsLargerOrEqualThanEarnedStars(int32 StarsToCheck) const
{
	return EarnedStars >= StarsToCheck;

}
