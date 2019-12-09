// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"

#include "KRVEnums.h"
#include "ActorComponents/KRVStatComponent.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "KRVInGameGameMode.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FOnAllGatesFinishSpawnInTheWaveDeleagte, float);
DECLARE_MULTICAST_DELEGATE(FOnAllGatesFinishAllSpawnDeleagte);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnNextWavePathsShownDelegate, bool);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnNextWaveCalledDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnNextWaveIncomingDelegate, float, CallTimeLimit);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnEndGameDelegate, EKRVTeam);
DECLARE_MULTICAST_DELEGATE(FOnGameStoppedAfterGameEndDelegate);

UCLASS()
class KRV_API AKRVInGameGameMode : public AGameModeBase, public IKRVDataAssetInterface
{
	GENERATED_BODY()

public:
	AKRVInGameGameMode();

public:
	virtual void PostInitializeComponents() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

public: // Asset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	virtual UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

	UFUNCTION(BlueprintCallable)
		UKRVLevelDataAsset* GetLevelDataAsset() const;

protected:
	UPROPERTY()
		class UKRVLevelDataAsset* LevelDataAsset;

protected:
	UFUNCTION(BlueprintCallable)
		class AKRVGameState* GetKRVGameState();

protected:
	UPROPERTY()
		class AKRVGameState* KRVGameState;

public: // Waves
	UFUNCTION(BlueprintCallable, Category = "Wave")
		int32 GetCurrentWave();

	UFUNCTION(BlueprintCallable)
		void CallIncoming(float CallTimeLimit = -1.f);

	UFUNCTION(BlueprintCallable)
		void CallNextWave();

	UFUNCTION(BlueprintCallable)
		void ShowNextWavePaths(bool bIsTrue);

	UFUNCTION()
		void OnTheGateFinishSpawnInTheWave(float CallTimeLimit, class AKRVGateway* TheGateWay);

	UFUNCTION()
		void OnTheGateFinishAllSpawn(class AKRVGateway* TheGateWay);

	void AddGate(AKRVGateway* NewGateWay);

protected:
	UFUNCTION(BlueprintCallable)
		void EndGame(EKRVTeam Winner);

	UFUNCTION()
		void StopGameAfterGameEnd();

	int32 GetStarsToEarn();

public:
	FOnAllGatesFinishSpawnInTheWaveDeleagte OnAllGatesFinishSpawnInTheWaveDeleagte;
	FOnAllGatesFinishAllSpawnDeleagte OnAllGatesFinishAllSpawnDeleagte;
	FOnNextWaveCalledDelegate OnNextWaveCalledDeleagte;
	FOnNextWaveIncomingDelegate OnNextWaveIncomingDelegate;
	FOnNextWavePathsShownDelegate OnNextWavePathsShownDelegate;
	FOnEndGameDelegate OnGameEndDelegate;
	FOnGameStoppedAfterGameEndDelegate OnGameStoppedAfterGameEndDelegate;

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Wave")
		int32 GatesSpawnInTheWaveFinished;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Wave")
		int32 GatesAllSpawnFinished;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Wave")
		TArray<class AKRVGateway*> Gateways;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		float GameStopDelayAfterGameEnd = 2.f;

	UPROPERTY(Transient, BlueprintReadOnly)
		bool bIsNextWaveIncoming;

	FTimerHandle GameStopTimerOnGameEnd;
	FTimerHandle NextWaveIncomingTimer;

public: // Item
	UFUNCTION()
		int32 GetRemainingStock(class AKRVItem* ItemToGet, class AKRVCombatUnit* NewCaster);

	UFUNCTION()
		int32 UseItem(class AKRVItem* ItemToUse, class AKRVCombatUnit* NewCaster);


public: // Unit
	UFUNCTION(BlueprintCallable)
		int32 IssueUnitInFieldID();

protected:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "ID")
		int32 RecentlyIssuedUnitInFieldID;

public: // Character
	UFUNCTION(BlueprintCallable)
		bool IsKiiledByPlayer(class AKRVCharacter* DeadCharacter);

	UFUNCTION()
		void OnEnemySpawned(class AKRVCharacter* SpwanedCharacter);

	UFUNCTION()
		void OnRecruited(class AKRVCharacter* SpwanedCharacter);

	UFUNCTION()
		void OnUnitDie(class AKRVCombatUnit* DeadUnit);

	UFUNCTION()
		void OnUnitDieWithReward(int32 NewGold, int32 NewExp, int32 NewRuby, class AKRVCombatUnit* DeadUnit);

	UFUNCTION()
		void OnEnemyEscape(class AKRVCharacter* EscapedEnemy);

protected:
	bool VariateEnemiesInField(int32 EnemiesToAdd);


public: // Tower
	UFUNCTION()
		void OnTowerSold(int32 Price, class AKRVTower* SoldTower);

public: // Add / subtract game resources ex) items
	UFUNCTION(BlueprintCallable)
		void AddHearts(int32 _Hearts);

	UFUNCTION(BlueprintCallable)
		void AddAvailableGold(int32 NewGold);

public: // Damage
	UFUNCTION(BlueprintCallable)
		float ApplyDamageAndPlayEffect(const FHitResult& SweepResult, float BaseDamage,
			AKRVCombatUnit* DamageCauser, TSubclassOf<UKRVDamageType> DamageTypeClass);

	UFUNCTION(BlueprintCallable)
		bool ApplyRadialDamageAndPlayEffect(float MinDamage, float MaxDamage, const FVector& DamageOrigin, float DamageRadius,
			AActor* DamageCauser, TSubclassOf<UKRVDamageType> DamageTypeClass, bool bDoesDrawDebugLine = false);

protected:
	UFUNCTION(BlueprintCallable)
		float ApplyDamage(AKRVCharacter* DamagedActor, float BaseDamage,
			AKRVCombatUnit* DamageCauser, TSubclassOf<UKRVDamageType> DamageTypeClass);

	UFUNCTION(BlueprintCallable)
		void PlayDamageTypeEffect(const FHitResult& _HitResult, TSubclassOf<UKRVDamageType> DamageTypeClass) const;

};
