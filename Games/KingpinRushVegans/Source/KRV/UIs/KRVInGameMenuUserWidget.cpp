// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVInGameMenuUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Slider.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "Controllers/KRVMapPlayerController.h"
#include "Controllers/KRVInGamePlayerController.h"

#include "KRVIconUserWidget.h"
#include "KRVConfirmUserWidget.h"

void UKRVInGameMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	ToMainMenuIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ToMainMenuIcon_1")));
	if (ToMainMenuIconWidget)
	{
		ToMainMenuIconWidget->SetCustomMenuIcon(EIconTextureVariation::LogOut, FText::GetEmpty(), FText::FromName(ReturnToMenuTooltipTextText));
		ToMainMenuIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVInGameMenuUserWidget::OnToMainMenuIconClicked);
	}	
	
	QuitGameIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_QuitGameIcon_1")));
	if (QuitGameIconWidget)
	{
		QuitGameIconWidget->SetCustomMenuIcon(EIconTextureVariation::ShutDown, FText::GetEmpty(), FText::FromName(QuitGameTooltipTextText));
		QuitGameIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVInGameMenuUserWidget::OnQuitGameIconClicked);
	}

	RestartLevelIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_RestartLevelIcon_1")));
	if (RestartLevelIconWidget)
	{
		RestartLevelIconWidget->SetCustomMenuIcon(EIconTextureVariation::Restore, FText::GetEmpty(), FText::FromName(ResetDataTooltipTextText));
		RestartLevelIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVInGameMenuUserWidget::OnRestartLevelconClicked);
	}

	CancleIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_CancleIcon_1")));
	if (CancleIconWidget)
	{
		CancleIconWidget->SetCustomMenuIcon(EIconTextureVariation::Cancle);
		CancleIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVInGameMenuUserWidget::OnCancleIconClicked);
	}

	VolumeSlider = Cast<USlider>(GetWidgetFromName(TEXT("SD_Volume_1")));
	if (VolumeSlider)
	{
		VolumeSlider->OnValueChanged.AddDynamic(this, &UKRVInGameMenuUserWidget::OnVolumeSliderValueChanged);
	}

	ResetDataIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ResetIcon_1")));
	if (ResetDataIconWidget)
	{
		ResetDataIconWidget->SetCustomMenuIcon(EIconTextureVariation::Restore);
		ResetDataIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVInGameMenuUserWidget::OnResetDataIconClicked);
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(ConfirmWidgetClassPath, FStreamableDelegate::CreateUObject(this,
		&UKRVInGameMenuUserWidget::OnConfirmWidgetClassAsyncLoaded));

}

void UKRVInGameMenuUserWidget::OnConfirmWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVConfirmUserWidget> AssetPathPtr(ConfirmWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		ConfirmWidgetClass = AssetPathPtr.Get();
	}

}

bool UKRVInGameMenuUserWidget::ShowConfirmWidget()
{
	if (!ConfirmWidget && ConfirmWidgetClass)
	{
		ConfirmWidget = CreateWidget<UKRVConfirmUserWidget>(this, ConfirmWidgetClass);
	}

	if (!ConfirmWidget)
	{
		KRVCHECK(ConfirmWidget);
	}
	else
	{
		ConfirmWidget->AddToViewport(5);
		ConfirmWidget->SetVisibility(ESlateVisibility::Visible);
		return true;
	}
	return false;

}

void UKRVInGameMenuUserWidget::OnToMainMenuIconClicked()
{
	if (ShowConfirmWidget())
	{
		ConfirmWidget->SetConfirmText(FText::FromName(ReturnToMenuConfirmText));

		if (ConfirmWidget->OnConfirmClickedDelegate.IsBound())
		{
			ConfirmWidget->OnConfirmClickedDelegate.Unbind();
		}
		ConfirmWidget->OnConfirmClickedDelegate.BindUObject(this, &UKRVInGameMenuUserWidget::ReturnToMainMenu);
	}

}

void UKRVInGameMenuUserWidget::ReturnToMainMenu()
{
	if (GetWorld())
	{
		AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		if (KRVInGamePlayerController)
		{
			KRVInGamePlayerController->ReturnToMainMenu();
		}
	}
	OnCancleIconClicked();

}

void UKRVInGameMenuUserWidget::OnQuitGameIconClicked()
{
	if (ShowConfirmWidget())
	{
		ConfirmWidget->SetConfirmText(FText::FromName(QuitGameConfirmText));

		if (ConfirmWidget->OnConfirmClickedDelegate.IsBound())
		{
			ConfirmWidget->OnConfirmClickedDelegate.Unbind();
		}
		ConfirmWidget->OnConfirmClickedDelegate.BindUObject(this, &UKRVInGameMenuUserWidget::QuitTheGame);
	}

}

void UKRVInGameMenuUserWidget::QuitTheGame()
{
	if (GetWorld())
	{
		AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		if (KRVInGamePlayerController)
		{
			KRVInGamePlayerController->QuitTheGame();
		}
	}

}

void UKRVInGameMenuUserWidget::OnRestartLevelconClicked()
{
	if (ShowConfirmWidget())
	{
		ConfirmWidget->SetConfirmText(FText::FromName(RestartLevelConfirmText));

		if (ConfirmWidget->OnConfirmClickedDelegate.IsBound())
		{
			ConfirmWidget->OnConfirmClickedDelegate.Unbind();
		}
		ConfirmWidget->OnConfirmClickedDelegate.BindUObject(this, &UKRVInGameMenuUserWidget::RestartTheLevel);
	}

}

void UKRVInGameMenuUserWidget::RestartTheLevel()
{
	if (GetWorld())
	{
		AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		if (KRVInGamePlayerController)
		{
			KRVInGamePlayerController->RestartTheLevel();
		}
	}
	OnCancleIconClicked();

}

void UKRVInGameMenuUserWidget::OnCancleIconClicked()
{
	SetVisibility(ESlateVisibility::Hidden);
	RemoveFromParent();
	if (GetWorld() && UGameplayStatics::IsGamePaused(GetWorld()))
	{
		UGameplayStatics::SetGamePaused(GetWorld(), false);
	}

	if (GetWorld())
	{
		AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		if (KRVInGamePlayerController)
		{
			KRVInGamePlayerController->SetGameInputMode(true);
		}
	}

}

void UKRVInGameMenuUserWidget::OnVolumeSliderValueChanged(float NewValue)
{
	if (VolumeSlider)
	{
		VolumeSlider->SetValue(NewValue);
	}

	if (GetWorld())
	{
		AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		if (KRVInGamePlayerController)
		{
			KRVInGamePlayerController->ChangeSoundVolume(NewValue);
		}
	}
}

void UKRVInGameMenuUserWidget::OnResetDataIconClicked()
{
	if (ShowConfirmWidget())
	{
		ConfirmWidget->SetConfirmText(FText::FromName(ResetDataConfirmText));

		if (ConfirmWidget->OnConfirmClickedDelegate.IsBound())
		{
			ConfirmWidget->OnConfirmClickedDelegate.Unbind();
		}
		ConfirmWidget->OnConfirmClickedDelegate.BindUObject(this, &UKRVInGameMenuUserWidget::ResetSaveData);
	}

}

void UKRVInGameMenuUserWidget::ResetSaveData()
{
	if (GetWorld())
	{
		AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
		if (!KRVMapPlayerController)
		{
			KRVCHECK(KRVMapPlayerController);
		}
		else
		{
			AKRVMapPlayerState* KRVMapPlayerState = KRVMapPlayerController->GetKRVMapPlayerState();
			if (KRVMapPlayerState)
			{
				KRVCHECK(KRVMapPlayerState->ResetSaveGame());
			}
		}
	}

}
