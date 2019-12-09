// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVLogBoxUserWidget.generated.h"

UCLASS()
class KRV_API UKRVLogBoxUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	void OnLogWidgetClassAsyncLoaded();
	void OnFallenTextureAsyncLoaded();
	void OnWarningTextureAsyncLoaded();
	void OnWaveTextureAsyncLoaded();
	void OnCrossedSwordsTextureAsyncLoaded();

protected:
	UPROPERTY(BlueprintReadOnly)
		TSubclassOf<class UKRVLogUserWidget> LogWidgetClass;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* FallenTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* WarningTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* WaveTexture;

	UPROPERTY(BlueprintReadOnly)
		class UTexture2D* CrossedSwordsTexture;

	FSoftClassPath LogWidgetPath = "/Game/Resources/UI/UI_InGameLog.UI_InGameLog_C";
	FSoftObjectPath FallenTexturePath = "/Game/Resources/Textures/T_Fallen.T_Fallen";
	FSoftObjectPath WarningTexturePath = "/Game/Resources/Textures/T_Warning.T_Warning";
	FSoftObjectPath WaveTexturePath = "/Game/Resources/Textures/T_Wave.T_Wave";
	FSoftObjectPath CrossedSwordsTexturePath = "/Game/Resources/Textures/T_CrossedSwords.T_CrossedSwords";

public:
	bool CreateLog(const FText& NewText, class UTexture2D* NewTexture = nullptr);

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ShowThisWidget();
	void ShowThisWidget_Implementation();

	UFUNCTION(BlueprintImplementableEvent)
		void HideThisWidget();

	UFUNCTION()
		void OnLogHidden(UKRVLogUserWidget* NewLogWidget);

protected:
	UPROPERTY(BlueprintReadOnly)
		class UUniformGridPanel* LogGridPanel;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UKRVLogUserWidget*> LogWidgets;

	UPROPERTY(BlueprintReadOnly)
		TArray<class UUniformGridSlot*> LogGridSlots;

	UPROPERTY(BlueprintReadWrite)
		int32 MaxLogsToShow = 5;

	int32 IndexToCreateLog;
	int32 ActiveLogCounts;

public: // Hero
	void BindToHero(class AKRVHero* NewHero);

protected:
	UFUNCTION()
		void OnReviveTimeNotified(float RemainingTime, float TotalTime);

	UFUNCTION()
		void OnLevelChanged(int32 NewLevel);

protected:
	FString OnHeroDieString = FString(TEXT("Our hero has slained. (Revives in {0}s)"));
	FString OnHeroLevelUpString = FString(TEXT("Reached to level {0}!"));

public:  // Bind To GameState
	void BindToGameState();

protected:
	UFUNCTION()
		void OnWaveChanged(int32 NewCurrentWave, int32 TotalWaves);

	UFUNCTION()
		void OnGameEnd(EKRVTeam Winnner);

	UFUNCTION()
		void OnInGameStateChanged(EGameState NewGameState);

protected:
	FString OnWaveChangedString = FString(TEXT("Wave {0} begins."));
	FString OnIncomingString = FString(TEXT("A new wave is incoming."));
	FString OnGameEndString = FString(TEXT("The game ends."));

};
