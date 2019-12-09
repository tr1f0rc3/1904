// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRVEnums.h"
#include "Structs/KRVAttackStat.h"
#include "Structs/KRVDefenceStat.h"
#include "Interfaces/KRVDataAssetInterface.h"
#include "KRVBuff.generated.h"

DECLARE_DELEGATE_RetVal_OneParam(bool, FOnThisBuffRemovedThisPtr, AKRVBuff*);
DECLARE_MULTICAST_DELEGATE(FMultiOnThisBuffRemoved);

class UPooledComponent;
class UKRVBuffDataAsset;
class AKRVCombatUnit;

UCLASS()
class KRV_API AKRVBuff : public AActor, public IKRVDataAssetInterface
{
	GENERATED_BODY()

public:
	AKRVBuff();

public:
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public: // DataAsset
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		class UKRVDataAsset* GetDataAsset() const;
	UKRVDataAsset* GetDataAsset_Implementation() const override;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		bool LoadFromDataAsset();
	virtual bool LoadFromDataAsset_Implementation() override;

	UFUNCTION(BlueprintCallable, Category = "Load")
		AKRVBuff* LoadStatData(FName _Name);

	UFUNCTION(BlueprintCallable, Category = "Load")
		UKRVBuffDataAsset* GetBuffDataAsset() const;

protected:
	void OnBuffParticleAsyncLoaded();
	void OnBuffSoundAsyncLoaded();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Load")
		TSoftObjectPtr<UKRVBuffDataAsset> MyDataAsset;

	UPROPERTY()
		UKRVDataAsset* HardPtr_MyDataAsset;

	bool bIsDataLoaded;
	FSoftObjectPath BuffParticlePath;
	FSoftObjectPath BuffSoundPath;

public: // Caster and Owner
	UFUNCTION(BlueprintCallable, Category = "Target")
		AKRVCombatUnit* GetBuffCaster() const;

public:
	UPROPERTY(Transient, BlueprintReadOnly, Category = "Target")
		AKRVCombatUnit* BuffOwner;

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		TWeakObjectPtr<AKRVCombatUnit> BuffCaster;

public: // Give/Remove/Overtime
	UFUNCTION(BlueprintCallable)
		bool GiveThis(AKRVCombatUnit* _Owner, AKRVCombatUnit* _Caster);

	// Do not call this directly; AKRVCombatUnit::RemoveBuff calls this
	UFUNCTION(BlueprintCallable)
		void RemoveThis();

protected:
	UFUNCTION() // just like delegate fcn; activates when buff ends
		void OnTimeUp();

	UFUNCTION()
		void ApplyOvertime();

public:
	FOnThisBuffRemovedThisPtr OnThisBuffRemovedThisPtr;
	FMultiOnThisBuffRemoved MultiOnThisBuffRemoved;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "ObjectPool")
		UPooledComponent* PooledComponent;

	bool bDoesBuffTaken;
	float AmountPerTick;
	float OvertimePeriod;
	int32 RemainingOvertimeTicks;

	FTimerHandle BuffRunoutTimerHandle;
	FTimerHandle OvertimeTimerHandle;
	FTimerHandle RemoveThisTimer;

public: // buff stats
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Team")
		ETargetType TargetType = ETargetType::NONE;

	// Lower spell level buff cannot overrides higher.
	// Higher spell level buff overrides lower.
	// 높은 레벨 버프만 낮거나 동등한 레벨 버프를 덮어쓸 수 있음.
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Duration")
		int32 SpellLevel = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Duration")
		float Duration = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stat")
		FKRVAttackStat AttackStat;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stat")
		FKRVDefenceStat DefenceStat;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stat")
		int32 MovementSpeed = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Stat")
		float MovementSpeedScale = 0.f;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Overtime")
		TSubclassOf<UDamageType> DOTDamageType = nullptr;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Overtime")
		int32 TotalOvertimeAmount = 0;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Overtime")
		int32 OvertimeTicks = 0;

protected:
	int32 RemainingTicks = 0;

public:
	void PlayBuffParticle();
	void PlayBuffSound();

protected: // Effects
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Particle")
		UParticleSystem* BuffParticle;

	UPROPERTY(Transient, BlueprintReadOnly, Category = "Particle")
		TArray<UParticleSystemComponent*> BuffParticleComponents;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "Sound")
		USoundCue* BuffSound;

	bool bDoesBuffParticlePlayed;
	bool bDoesBuffSoundPlayed;


public: // Operators
	UFUNCTION(BlueprintCallable)
		AKRVBuff* AssignmentOperator(const AKRVBuff* const Other);

	FORCEINLINE AKRVBuff& operator+=(const AKRVBuff& Other)
	{
		AttackStat += Other.AttackStat;
		DefenceStat += Other.DefenceStat;
		MovementSpeed += Other.MovementSpeed;
		MovementSpeedScale += Other.MovementSpeedScale;
		return *this;

	}

};
