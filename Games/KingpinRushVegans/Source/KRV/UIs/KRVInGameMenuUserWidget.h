// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVInGameMenuUserWidget.generated.h"

UCLASS()
class KRV_API UKRVInGameMenuUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected: // Load
	void OnConfirmWidgetClassAsyncLoaded();

protected:
	FSoftClassPath ConfirmWidgetClassPath = "/Game/Resources/UI/UI_Confirmation.UI_Confirmation_C";

protected: // Confirm
	bool ShowConfirmWidget();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		TSubclassOf<class UKRVConfirmUserWidget> ConfirmWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		UKRVConfirmUserWidget* ConfirmWidget;

protected: // Return to main menu
	UFUNCTION()
		void OnToMainMenuIconClicked();

	UFUNCTION()
		void ReturnToMainMenu();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* ToMainMenuIconWidget;

	FName ReturnToMenuTooltipTextText = "Restart to main menu.";
	FName ReturnToMenuConfirmText = "Are you sure to return to main menu?";

protected: // Quit the game
	UFUNCTION()
		void OnQuitGameIconClicked();

	UFUNCTION()
		void QuitTheGame();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* QuitGameIconWidget;

	FName QuitGameTooltipTextText = "Quit the game.";
	FName QuitGameConfirmText = "Are you sure to quit the game?";

protected: // Restart game
	UFUNCTION()
		void OnRestartLevelconClicked();

	UFUNCTION()
		void RestartTheLevel();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* RestartLevelIconWidget;

	FName RestartLevelTooltipTextText = "Restart the level.";
	FName RestartLevelConfirmText = "Are you sure to restart the level?";

protected: // Cancle
	UFUNCTION()
		void OnCancleIconClicked();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* CancleIconWidget;

protected: // Volume Change
	UFUNCTION()
		void OnVolumeSliderValueChanged(float NewValue);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class USlider* VolumeSlider;

protected: // Reset Data
	UFUNCTION()
		void OnResetDataIconClicked();

	UFUNCTION()
		void ResetSaveData();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVIconUserWidget* ResetDataIconWidget;

	FName ResetDataTooltipTextText = "Reset all the save data.";
	FName ResetDataConfirmText = "Are you sure to reset the save data?";

};
