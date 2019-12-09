// Fill out your copyright notice in the Description page of Project Settings.

#include "Controllers/KRVPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundMix.h"
#include "Sound/SoundClass.h"
#include "Sound/SoundCue.h"
#include "UserWidget.h"

#include "KRV.h"
#include "Pawns/KRVPlayerCameraPawn.h"
#include "SaveGames/KRVGameSetting.h"

AKRVPlayerController::AKRVPlayerController()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> LOADING_SCREEN_UI_C
	(TEXT("/Game/Resources/UI/UI_LoadingScreen.UI_LoadingScreen_C"));
	if (LOADING_SCREEN_UI_C.Succeeded())
	{
		LoadingScreenWidgetClass = LOADING_SCREEN_UI_C.Class;
	}

	static ConstructorHelpers::FObjectFinder<USoundClass> SOUND_CLASS_OBJ
	(TEXT("/Engine/EngineSounds/Master.Master"));
	if (SOUND_CLASS_OBJ.Succeeded())
	{
		SoundClassToSet = SOUND_CLASS_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundMix> SOUND_MIX_OBJ
	(TEXT("/Game/Resources/Sound/KRVSoundMix.KRVSoundMix"));
	if (SOUND_MIX_OBJ.Succeeded())
	{
		SoundMixToSet = SOUND_MIX_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> ON_PRESSED_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Menu_UI_Beeps/Cues/retro_ui_menu_simple_click_01_Cue.retro_ui_menu_simple_click_01_Cue"));
	if (ON_PRESSED_SOUND_OBJ.Succeeded())
	{
		OnPressedSound = ON_PRESSED_SOUND_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> REJECTED_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Alarms_Sirens/Cues/retro_alarm_siren_loop_20_Cue.retro_alarm_siren_loop_20_Cue"));
	if (REJECTED_SOUND_OBJ.Succeeded())
	{
		RejectedSound = REJECTED_SOUND_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> UPGRADE_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Powerup/Cues/retro_powerup_collect_29_Cue.retro_powerup_collect_29_Cue"));
	if (UPGRADE_SOUND_OBJ.Succeeded())
	{
		UpgradeSound = UPGRADE_SOUND_OBJ.Object;
	}
	
	static ConstructorHelpers::FObjectFinder<USoundCue> WAVE_CALL_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Miscellaneous/Cues/retro_misc_various_sounds_87_Cue.retro_misc_various_sounds_87_Cue"));
	if (WAVE_CALL_SOUND_OBJ.Succeeded())
	{
		WaveCallSound = WAVE_CALL_SOUND_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> ON_HIGHLIGHTED_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Menu_UI_Beeps/Cues/retro_ui_menu_popup_04_Cue.retro_ui_menu_popup_04_Cue"));
	if (ON_HIGHLIGHTED_SOUND_OBJ.Succeeded())
	{
		OnHighlightedSound = ON_HIGHLIGHTED_SOUND_OBJ.Object;
	}

}

void AKRVPlayerController::OnPossess(APawn* NewPawn)
{
	Super::OnPossess(NewPawn);

	KRVPlayerCameraPawn = Cast<AKRVPlayerCameraPawn>(NewPawn);

}

void AKRVPlayerController::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AKRVPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	SetMouseCursorInTick();

}

void AKRVPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

}

void AKRVPlayerController::BeginPlay()
{
	Super::BeginPlay();

	SetGameInputMode(false);

}

void AKRVPlayerController::ChangeSoundVolume(float NewVolume)
{
	if (!LoadGameSetting())
	{
		KRVCHECK(LoadedGameSetting);
	}
	else
	{
		if (NewVolume >= 0.f)
		{
			NewVolume = FMath::Clamp(NewVolume, 0.f, 1.f);
			LoadedGameSetting->SoundVolume = NewVolume;
			UGameplayStatics::SaveGameToSlot(LoadedGameSetting, SettingSlotString, 0);
		}
		else
		{
			NewVolume = LoadedGameSetting->SoundVolume;
		}

		if (!SoundClassToSet || !SoundMixToSet)
		{
			KRVCHECK(SoundClassToSet);
			KRVCHECK(SoundMixToSet);
		}
		else
		{
			UGameplayStatics::SetSoundMixClassOverride(this, SoundMixToSet, SoundClassToSet, NewVolume, 1.f, 1.f, true);
			UGameplayStatics::PushSoundMixModifier(this, SoundMixToSet);
		}
	}

}

bool AKRVPlayerController::LoadGameSetting()
{
	if (!LoadedGameSetting)
	{
		LoadedGameSetting = Cast<UKRVGameSetting>
			(UGameplayStatics::LoadGameFromSlot(SettingSlotString, 0));
	}
	if (!LoadedGameSetting)
	{
		LoadedGameSetting = GetMutableDefault<UKRVGameSetting>();
	}

	if (LoadedGameSetting)
	{
		return true;
	}
	else
	{
		return false;
	}

}

bool AKRVPlayerController::PlayOnPressedSound()
{
	if (OnPressedSound && GetWorld())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), OnPressedSound);
		return true;
	}
	return false;

}

bool AKRVPlayerController::PlayRejectedSound()
{
	if (RejectedSound && GetWorld())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), RejectedSound);
		return true;
	}
	return false;

}

bool AKRVPlayerController::PlayUpgradeSound()
{
	if (UpgradeSound && GetWorld())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), UpgradeSound);
		return true;
	}
	return false;

}

bool AKRVPlayerController::PlayWaveCallSound()
{
	if (WaveCallSound && GetWorld())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), WaveCallSound);
		return true;
	}
	return false;

}

bool AKRVPlayerController::PlayOnHightlightedSound()
{
	if (OnHighlightedSound && GetWorld())
	{
		UGameplayStatics::PlaySound2D(GetWorld(), OnHighlightedSound);
		return true;
	}
	return false;

}

void AKRVPlayerController::QuitTheGame()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), Cast<APlayerController>(this), EQuitPreference::Quit, false);

}

void AKRVPlayerController::RestartTheLevel()
{
	FadeInTheScreen(false);
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(FadingTimer, this,
			&AKRVPlayerController::RestartTheLevelAfterFadeOut, FadeDuration, false);
	}

}

void AKRVPlayerController::OpenLevelByName(const FName& LevelNameToOpen)
{
	ShowLoadingScreen();
	if (GetWorld())
	{
		UGameplayStatics::OpenLevel(GetWorld(), LevelNameToOpen);
	}

}

void AKRVPlayerController::FadeInTheScreen(bool bIsFadeIn)
{
	if (GetWorld())
	{
		if (PlayerCameraManager)
		{
			float FromAlpha;
			float ToAlpha;
			if (bIsFadeIn)
			{
				FromAlpha = 1.f;
				ToAlpha = 0.f;
			}
			else
			{
				FromAlpha = 0.f;
				ToAlpha = 1.f;
			}
			FLinearColor LinearFadeColor(0.f, 0.f, 0.f);
			PlayerCameraManager->StartCameraFade(FromAlpha, ToAlpha, FadeDuration, LinearFadeColor, true, false);
		}
	}
}

void AKRVPlayerController::ShowLoadingScreen()
{
	if (LoadingScreenWidgetClass)
	{
		UUserWidget* LoadingScreenWidget = CreateWidget<UUserWidget>(this, LoadingScreenWidgetClass);
		if (LoadingScreenWidget)
		{
			LoadingScreenWidget->AddToViewport(4);
			LoadingScreenWidget->SetVisibility(ESlateVisibility::Visible);
		}
	}
}

void AKRVPlayerController::RestartTheLevelAfterFadeOut()
{
	ShowLoadingScreen();
	RestartLevel();

}

void AKRVPlayerController::SetGameInputMode(bool bIsInputModeGame)
{
	bShowMouseCursor = true;

	if (bIsInputModeGame)
	{
		FInputModeGameAndUI InputMode;
		CurrentInputMode = EKRVInputMode::Game;
		SetInputMode(InputMode);
	}
	else
	{
		FInputModeUIOnly InputMode;
		CurrentInputMode = EKRVInputMode::UI;
		SetInputMode(InputMode);
	}

}

void AKRVPlayerController::SetMouseCursorInTick()
{
	if (!GetWorld() || !bShowMouseCursor)
	{
		return;
	}
	else
	{
		EKRVCursor NewKRVCursor = EKRVCursor::Default;
		if (KRVPlayerCameraPawn)
		{
			if (KRVPlayerCameraPawn->GetCameraPanDirection() != FVector::ZeroVector)
			{
				NewKRVCursor = EKRVCursor::Panning;
			}
		}

		CurrentKRVCursor = NewKRVCursor;
		switch (CurrentKRVCursor)
		{
		case EKRVCursor::Default:
		{
			CurrentMouseCursor = EMouseCursor::Default;
			break;
		}
		case EKRVCursor::Panning:
		{
			CurrentMouseCursor = EMouseCursor::CardinalCross;
			break;
		}
		default:
			break;
		}
	}

}

bool AKRVPlayerController::AddWidgetFromViewport(class UUserWidget* WidgetToShow, bool bIsTrue, int32 NewZOrder /*= 1*/)
{
	if (!WidgetToShow)
	{
		KRVCHECK(WidgetToShow);
		return false;
	}
	else if (bIsTrue)
	{
		WidgetToShow->AddToViewport(NewZOrder);
	}
	else
	{
		WidgetToShow->RemoveFromParent();
	}
	return true;

}
