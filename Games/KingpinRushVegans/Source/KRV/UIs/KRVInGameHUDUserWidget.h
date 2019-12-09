// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVInGameHUDUserWidget.generated.h"

UCLASS()
class KRV_API UKRVInGameHUDUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	UFUNCTION()
		void OnHeartChanged(int32 NewHearts);

	UFUNCTION()
		void OnGoldChanged(int32 NewGolds);

	UFUNCTION()
		void OnWaveChanged(int32 NewCurrentWave, int32 TotalWaves);

	UFUNCTION()
		void SetTimeText();

	UFUNCTION()
		void OnGameEnd(EKRVTeam Winnner);

	void BindToGameState();

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UTextBlock* HeartTextBlock;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UTextBlock* GoldTextBlock;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UTextBlock* WaveTextBlock;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UTextBlock* TimeTextBlock;

	UPROPERTY(Transient, BlueprintReadOnly)
		class AKRVGameState* BoundGameState;

	UPROPERTY(Transient, BlueprintReadOnly)
		float SecondsAtFirstWaveCalling;

	FTimerHandle PerSecondTimer;

};
