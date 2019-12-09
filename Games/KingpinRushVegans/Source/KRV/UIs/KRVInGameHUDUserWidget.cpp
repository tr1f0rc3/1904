// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVInGameHUDUserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TextBlock.h"
#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "KRVGameState.h"
#include "GameModes/KRVInGameGameMode.h"

void UKRVInGameHUDUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetVisibility(ESlateVisibility::HitTestInvisible);
	HeartTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Heart_1")));
	GoldTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Gold_1")));
	WaveTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Wave_1")));
	TimeTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Time_1")));
	BindToGameState();


}

void UKRVInGameHUDUserWidget::BindToGameState()
{
	if (GetWorld())
	{
		AKRVGameState* NewGameState = UKRVBlueprintFunctionLibrary::GetKRVGameState(GetWorld());
		if (!NewGameState)
		{
			KRVCHECK(NewGameState);
		}
		else
		{
			BoundGameState = NewGameState;
			NewGameState->OnGoldChangedDelegate.AddUObject(this, &UKRVInGameHUDUserWidget::OnGoldChanged);
			OnGoldChanged(NewGameState->GetAvailableGold());
			NewGameState->OnHeartChangedDelegate.AddUObject(this, &UKRVInGameHUDUserWidget::OnHeartChanged);
			OnHeartChanged(NewGameState->GetHeartsRemaining());
			NewGameState->OnWaveChangedDelegate.AddUObject(this, &UKRVInGameHUDUserWidget::OnWaveChanged);
			OnWaveChanged(NewGameState->GetCurrentWave(), NewGameState->GetTotalWaves());
		}

		AKRVInGameGameMode* NewGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		if (!NewGameMode)
		{
			KRVCHECK(NewGameMode);
		}
		else
		{
			NewGameMode->OnGameEndDelegate.AddUObject(this, &UKRVInGameHUDUserWidget::OnGameEnd);
		}

	}
}

void UKRVInGameHUDUserWidget::OnHeartChanged(int32 NewHearts)
{
	if (!HeartTextBlock)
	{
		KRVCHECK(HeartTextBlock);
	}
	else
	{
		HeartTextBlock->SetText(FText::FromString(FString::FromInt(NewHearts)));
	}

}

void UKRVInGameHUDUserWidget::OnGoldChanged(int32 NewGolds)
{
	if (!GoldTextBlock)
	{
		KRVCHECK(GoldTextBlock);
	}
	else
	{
		GoldTextBlock->SetText(FText::FromString(FString::FromInt(NewGolds)));
	}

}

void UKRVInGameHUDUserWidget::OnWaveChanged(int32 NewCurrentWave, int32 TotalWaves)
{
	if (!WaveTextBlock)
	{
		KRVCHECK(WaveTextBlock);
	}
	else
	{
		// Time
		GetWorld()->GetTimerManager().ClearTimer(PerSecondTimer);
		SetTimeText();
		GetWorld()->GetTimerManager().SetTimer(PerSecondTimer, this, &UKRVInGameHUDUserWidget::SetTimeText, 1.f, true);

		// Wave
		FString NewWaveString = FString::FromInt(FMath::Clamp(NewCurrentWave, 0 , TotalWaves)) + FString(TEXT("/")) + FString::FromInt(TotalWaves);
		WaveTextBlock->SetText(FText::FromString(NewWaveString));
	}

}

void UKRVInGameHUDUserWidget::SetTimeText()
{
	if (TimeTextBlock && GetWorld())
	{
		if (BoundGameState)
		{
			int32 SecondsAfterFirstWaveCalling = FMath::FloorToInt(BoundGameState->GetSecondsAfterFirstWaveCalling());
			FString StringToShow;
			UKRVBlueprintFunctionLibrary::GetStringFromSeconds(StringToShow, SecondsAfterFirstWaveCalling);
			TimeTextBlock->SetText(FText::FromString(StringToShow));
		}
	}

}

void UKRVInGameHUDUserWidget::OnGameEnd(EKRVTeam Winnner)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(PerSecondTimer);
	}

}
