// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "KRVPlayerController.generated.h"

UENUM(BlueprintType)
enum class EKRVInputMode : uint8
{
	NONE,
	Game,
	UI,
	MAX,

};

UENUM(BlueprintType)
enum class EKRVCursor : uint8
{
	NONE,
	Default,
	Panning,
	PlayerUnit,
	EnemyUnit,
	Targeting,
	MAX,

};
/**
 * 
 */
UCLASS()
class KRV_API AKRVPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AKRVPlayerController();

public:
	virtual void OnPossess(APawn* NewPawn) override;
	virtual void PostInitializeComponents() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

protected:
	virtual void BeginPlay() override;

public: // Game Setting
	UFUNCTION(BlueprintCallable)
		void ChangeSoundVolume(float NewVolume);

	bool LoadGameSetting();

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVGameSetting* LoadedGameSetting;

	UPROPERTY(BlueprintReadOnly)
		class USoundClass* SoundClassToSet;

	UPROPERTY(BlueprintReadOnly)
		class USoundMix* SoundMixToSet;

	FString SettingSlotString = "DefaultSetting";

public: // Sound
	bool PlayOnPressedSound();
	bool PlayRejectedSound();
	bool PlayUpgradeSound();
	bool PlayWaveCallSound();
	bool PlayOnHightlightedSound();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class USoundCue* OnPressedSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class USoundCue* RejectedSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class USoundCue* UpgradeSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class USoundCue* WaveCallSound;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class USoundCue* OnHighlightedSound;


public: // Level
	UFUNCTION(BlueprintCallable)
		void QuitTheGame();

	UFUNCTION(BlueprintCallable)
		void RestartTheLevel();

protected:
	UFUNCTION(BlueprintCallable)
		void FadeInTheScreen(bool bIsFadeIn);

	UFUNCTION(BlueprintCallable)
		void ShowLoadingScreen();

	UFUNCTION()
		void RestartTheLevelAfterFadeOut();

	void OpenLevelByName(const FName& LevelNameToOpen);


protected:
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly)
		float FadeDuration = 1.f;

	UPROPERTY(BlueprintReadOnly, VisibleInstanceOnly)
		TSubclassOf<UUserWidget> LoadingScreenWidgetClass;

	FTimerHandle FadingTimer;
	FName MainMenuLevelName = "MainMenu";

public: // Input
	void SetGameInputMode(bool bIsInputModeGame);

protected:
	virtual void SetMouseCursorInTick();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EKRVInputMode CurrentInputMode = EKRVInputMode::Game;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EKRVCursor CurrentKRVCursor = EKRVCursor::Default;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class AKRVPlayerCameraPawn* KRVPlayerCameraPawn;

protected:
	bool AddWidgetFromViewport(class UUserWidget* WidgetToShow, bool bIsTrue, int32 NewZOrder = 1);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class TSubclassOf<UUserWidget> InGamePauseWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UUserWidget* InGamePauseWidget;

};
