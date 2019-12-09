// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"

#include "KRVEnums.h"
#include "KRVGameState.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnInGameStateChangeDelegate, EGameState);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnWaveChangedDelegate, int32, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnHeartChangedDelegate, int32);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnGoldChangedDelegate, int32);

class AKRVInGamePlayerState;
class AKRVCharacter;
class AKRVInGamePlayerController;

UCLASS()
class KRV_API AKRVGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AKRVGameState();

public: // Wave 
	UFUNCTION(BlueprintCallable, Category = "Wave")
		int32 GetCurrentWave() const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
		int32 GetTotalWaves() const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
		EGameState GetGameStateEnum() const;

	UFUNCTION(BlueprintCallable, Category = "Wave")
		float GetSecondsAfterFirstWaveCalling();

	int32 JumpToNextWave(int32 WavesToJump = 1);
	void SetGameStateEnum(EGameState NewState);
	void SetTotalWaves(int32 NewWaves);
	bool IsLastWave() const;

public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Wave")
		int32 EnemiesInField;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Wave")
		TArray<float> SecondsAtWaveCalled;

	FOnWaveChangedDelegate OnWaveChangedDelegate;
	FOnInGameStateChangeDelegate OnInGameStateChangeDelegate;

protected:
	UPROPERTY(BlueprintReadWrite, Category = "Wave")
		EGameState InGameState = EGameState::Preparing;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Wave")
		int32 TotalWaves;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Wave")
		int32 CurrentWave;

public: // Resource
	UFUNCTION(BlueprintCallable)
		void GainKillRewards(AKRVInGamePlayerController* NewPC, int32 GoldToEarn, int32 ExpToGet, int32 RubyToEarn = 0, int32 KillCountsToAdd = 1);

	UFUNCTION(BlueprintCallable)
		int32 GetAvailableGold() const;

	UFUNCTION(BlueprintCallable)
		void AddGold(int32 GoldToAdd);

	UFUNCTION(BlueprintCallable)
		bool SpendGold(AKRVInGamePlayerController* NewPC, int32 NewGold);

	UFUNCTION(BlueprintCallable)
		void EarnRuby(AKRVInGamePlayerController* NewPC, int32 RubyToAdd);

	UFUNCTION(BlueprintCallable)
		int32 GetHeartsRemaining() const;

	UFUNCTION(BlueprintCallable, Category = "Enemy || Escape")
		int32 AddHearts(int32 HeartsToAdd);

	void SetHearts(int32 HeartsToSet);

public:
	FOnGoldChangedDelegate OnGoldChangedDelegate;
	FOnHeartChangedDelegate OnHeartChangedDelegate;

protected: // Gameplay variables
	UPROPERTY(BlueprintReadOnly, Category = "Resource")
		int32 HeartsRemaining = 20;

	// Gold is resource to upgrade and build tower in game.
	UPROPERTY(BlueprintReadOnly, Category = "Resource")
		int32 AvailableGold;

public: // Game Results
	bool SetGameResultOfPlayerState(class AKRVInGamePlayerController* NewPC, EKRVTeam NewWinner, int32 NewEarnedStars = 0);
	bool SavePlayerState(class AKRVInGamePlayerController* NewPC);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Results")
		EKRVTeam GameWinner = EKRVTeam::NONE;

public: // Statistics
	UFUNCTION(BlueprintCallable)
		void AddRecruits(AKRVInGamePlayerController* NewPC, int32 _Recruits = 1);

	UFUNCTION(BlueprintCallable)
		int32 AddEscapedEnemies(int32 _Enemies = 1);

	UFUNCTION(BlueprintCallable)
		int32 GetEarnedRuby() const;

	UFUNCTION(BlueprintCallable)
		int32 GetEarnedStars() const;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Statistics")
		int32 TotalKills;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Statistics")
		int32 EscapedEnemies;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Results")
		int32 EarnedRuby;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Results")
		int32 EarnedStars;

};
