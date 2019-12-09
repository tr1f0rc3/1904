// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVLogBoxUserWidget.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "UniformGridPanel.h"
#include "UniformGridSlot.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"

#include "PlayerStates/KRVInGamePlayerState.h"
#include "GameModes/KRVInGameGameMode.h"
#include "KRVGameState.h"
#include "Controllers/KRVInGamePlayerController.h"

#include "KRVLogUserWidget.h"
#include "Pawns/KRVHero.h"

void UKRVLogBoxUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	LogGridPanel = Cast<UUniformGridPanel>(GetWidgetFromName(TEXT("UG_Log_1")));
	if (LogGridPanel)
	{
		Streamable.RequestAsyncLoad(LogWidgetPath, FStreamableDelegate::CreateUObject(this,
			&UKRVLogBoxUserWidget::OnLogWidgetClassAsyncLoaded));
	}

	Streamable.RequestAsyncLoad(FallenTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVLogBoxUserWidget::OnFallenTextureAsyncLoaded));
	Streamable.RequestAsyncLoad(WarningTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVLogBoxUserWidget::OnWarningTextureAsyncLoaded));
	Streamable.RequestAsyncLoad(WaveTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVLogBoxUserWidget::OnWaveTextureAsyncLoaded));
	Streamable.RequestAsyncLoad(CrossedSwordsTexturePath, FStreamableDelegate::CreateUObject(this,
		&UKRVLogBoxUserWidget::OnCrossedSwordsTextureAsyncLoaded));

	SetVisibility(ESlateVisibility::Hidden);
	IndexToCreateLog = MaxLogsToShow - 1;
	ActiveLogCounts = 0;

	BindToGameState();

}

void UKRVLogBoxUserWidget::OnLogWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVLogUserWidget> AssetPathPtr(LogWidgetPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		LogWidgetClass = AssetPathPtr.Get();
		if (LogWidgetClass && LogGridPanel)
		{
			for (int32 Index = 0; Index < MaxLogsToShow; ++Index)
			{
				UKRVLogUserWidget* CreatedLogWidget = Cast<UKRVLogUserWidget>
					(CreateWidget<UUserWidget>(this, LogWidgetClass));
				if (CreatedLogWidget)
				{
					CreatedLogWidget->LogSlotIndex = Index;
					CreatedLogWidget->OnThisWidgetHiddenDelegate.BindUObject(this, &UKRVLogBoxUserWidget::OnLogHidden);
					CreatedLogWidget->SetVisibility(ESlateVisibility::Collapsed);
					LogWidgets.Emplace(CreatedLogWidget);

					UUniformGridSlot* LogGridSlot = LogGridPanel->AddChildToUniformGrid(CreatedLogWidget);
					LogGridSlot->SetColumn(0);
					LogGridSlot->SetRow(Index);
					LogGridSlots.Emplace(LogGridSlot);
				}
			}
		}
	}

}

void UKRVLogBoxUserWidget::OnFallenTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(FallenTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		FallenTexture = AssetPathPtr.Get();
	}

}

void UKRVLogBoxUserWidget::OnWarningTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(WarningTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		WarningTexture = AssetPathPtr.Get();
	}

}

void UKRVLogBoxUserWidget::OnWaveTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(WaveTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		WaveTexture = AssetPathPtr.Get();
	}

}

void UKRVLogBoxUserWidget::OnCrossedSwordsTextureAsyncLoaded()
{
	TSoftObjectPtr<UTexture2D> AssetPathPtr(CrossedSwordsTexturePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		CrossedSwordsTexture = AssetPathPtr.Get();
	}

}

bool UKRVLogBoxUserWidget::CreateLog(const FText& NewText, class UTexture2D* NewTexture /*= nullptr*/)
{
	if (LogWidgets.Num() != MaxLogsToShow)
	{
		KRVCHECK(LogWidgets.Num() == MaxLogsToShow);
	}
	else
	{
		UKRVLogUserWidget* LogWidgetToHandle = LogWidgets[IndexToCreateLog];
		if (!LogWidgetToHandle)
		{
			KRVCHECK(LogWidgetToHandle);
		}
		else
		{
			if (LogWidgetToHandle->GetVisibility() != ESlateVisibility::Collapsed)
			{
				LogWidgetToHandle->SetVisibility(ESlateVisibility::Collapsed);
			}

			if (LogWidgetToHandle->PopupLog(NewText, NewTexture))
			{
				for (UUniformGridSlot* LogGridSlot : LogGridSlots)
				{
					LogGridSlot->SetRow(LogGridSlot->Row + 1);
				}
				LogGridSlots[IndexToCreateLog]->SetRow(0);

				if (GetVisibility() == ESlateVisibility::Hidden)
				{
					ShowThisWidget();
				}

				if (++IndexToCreateLog > MaxLogsToShow - 1)
				{
					IndexToCreateLog = 0;
				}
				++ActiveLogCounts;
				return true;
			}
		}
	}
	return false;

}

void UKRVLogBoxUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

}

void UKRVLogBoxUserWidget::OnLogHidden(UKRVLogUserWidget* NewLogWidget)
{
	if (--ActiveLogCounts == 0)
	{
		KRVCHECK(GetVisibility() == ESlateVisibility::HitTestInvisible);
		HideThisWidget();
	}
	KRVCHECK(ActiveLogCounts >= 0);

}

void UKRVLogBoxUserWidget::OnReviveTimeNotified(float RemainingTime, float TotalTime)
{
	if (RemainingTime >= TotalTime - KINDA_SMALL_NUMBER)
	{
		CreateLog(FText::FormatOrdered(FTextFormat::FromString(OnHeroDieString), TotalTime), FallenTexture);
	}

}

void UKRVLogBoxUserWidget::OnLevelChanged(int32 NewLevel)
{
	CreateLog(FText::FormatOrdered(FTextFormat::FromString(OnHeroLevelUpString), NewLevel), CrossedSwordsTexture);

}

void UKRVLogBoxUserWidget::BindToHero(AKRVHero* NewHero)
{
	if (!NewHero)
	{
		KRVCHECK(NewHero);
	}
	else
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
		if (LogGridPanel && KRVInGamePlayerState)
		{
			KRVInGamePlayerState->OnLevelUpDelegate.AddUObject(this, &UKRVLogBoxUserWidget::OnLevelChanged);
		}

		NewHero->ReviveTimeNotificationDelegate.AddUObject(this, &UKRVLogBoxUserWidget::OnReviveTimeNotified);
	}

}

void UKRVLogBoxUserWidget::BindToGameState()
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
			NewGameState->OnWaveChangedDelegate.AddUObject(this, &UKRVLogBoxUserWidget::OnWaveChanged);
			NewGameState->OnInGameStateChangeDelegate.AddUObject(this, &UKRVLogBoxUserWidget::OnInGameStateChanged);
		}

		AKRVInGameGameMode* NewGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		if (!NewGameMode)
		{
			KRVCHECK(NewGameMode);
		}
		else
		{
			NewGameMode->OnGameEndDelegate.AddUObject(this, &UKRVLogBoxUserWidget::OnGameEnd);
		}
	}
}



void UKRVLogBoxUserWidget::OnWaveChanged(int32 NewCurrentWave, int32 TotalWaves)
{
	CreateLog(FText::FormatOrdered(FTextFormat::FromString(OnWaveChangedString), NewCurrentWave), WaveTexture);

}

void UKRVLogBoxUserWidget::OnGameEnd(EKRVTeam Winnner)
{
	CreateLog(FText::FromString(OnGameEndString), CrossedSwordsTexture);

}

void UKRVLogBoxUserWidget::OnInGameStateChanged(EGameState NewGameState)
{
	switch (NewGameState)
	{
	case EGameState::Incoming:
	{
		CreateLog(FText::FromString(OnIncomingString), WarningTexture);
		break;
	}
	default:
		break;
	}

}


