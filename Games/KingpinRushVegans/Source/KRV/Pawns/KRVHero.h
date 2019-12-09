// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pawns/KRVCharacter.h"
#include "KRVHero.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FReviveTimeNotificationDelegate, float, float);

UCLASS()
class KRV_API AKRVHero : public AKRVCharacter
{
	GENERATED_BODY()

public:
	AKRVHero();

public:
	void PostInitializeComponents() override;

protected:
	void BeginPlay() override;

public: // DataAsset
	virtual bool LoadBaseStat() override;

	UFUNCTION(BlueprintCallable)
		class UKRVHeroDataAsset* GetHeroDataAsset() const;

	bool LoadSkillMasteryLevels();

protected: // GameState
	bool UpdateGameStateExpData(int32 NewLevel);

protected: // Spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USpawnerComponent* SpawnerComponent;

protected: // Stat
	UFUNCTION()
		void OnLevelUp(int32 NewLevel);

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exp")
		UParticleSystemComponent* LevelUpParticleComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Exp")
		UAudioComponent* LevelUpAudioComponent;

public: // Die
	virtual bool Die() override;

protected:
	UFUNCTION(BlueprintCallable)
		void Revive();

	UFUNCTION(BlueprintCallable)
		float GetRemainingReviveTime();

public:
	FReviveTimeNotificationDelegate ReviveTimeNotificationDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Die")
		float InvincibleTimeWhenRevive = 3.f;

	FTimerHandle ReviveTimer;

};
