// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "KRVEnums.h"
#include "ActorComponents/KRVStatComponent.h"
#include "Structs/KRVDefenceStat.h"
#include "KRVCharacterStatComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHpChangedDelegate, float, float);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHpChangedDynamicDelegate, float, CurrentHealth, float, MaxHealth);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KRV_API UKRVCharacterStatComponent : public UKRVStatComponent
{
	GENERATED_BODY()

public:
	UKRVCharacterStatComponent();

public:

protected:
	virtual void BeginPlay() override;


public:
	virtual UKRVStatComponent* LoadData(const FName& UnitName) override;

public: // Health
	UFUNCTION(BlueprintCallable, Category = "Health")
		void InitHealth();

	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetHealth() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
		float GetHealthRatio() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
		float SetHealth(float _Health);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float LoseHealth(float _Health);

	UFUNCTION(BlueprintCallable, Category = "Health")
		float GainHealth(float _Health);


protected: // Defence
	// Only meaning to modified stat, not base stat
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
		float CurrentHealth;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
		FKRVDefenceStat DefenceStat;


public: // Melee
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat")
		FKRVAttackStat MeleeAttackStat;

public: // Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat || Move")
		int32 MovementSpeed;


public: // Enemy variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat || Reward")
		int32 LivesTaken;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat || Reward")
		int32 Bounty;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stat || Reward")
		int32 ExpGiven;

public:
	FOnHpChangedDelegate OnHPChangedDelegate;

	UPROPERTY(BlueprintAssignable)
		FOnHpChangedDynamicDelegate OnHpChangedDynamicDelegate;

public:
	UKRVCharacterStatComponent* AssignmentOperator(const UKRVCharacterStatComponent* const Other);

	FORCEINLINE UKRVCharacterStatComponent& operator+=(const AKRVBuff& Other)
	{
		*(Cast<UKRVStatComponent>(this)) += Other;
		MeleeAttackStat += Other.AttackStat;
		DefenceStat += Other.DefenceStat;
		MovementSpeed += Other.MovementSpeed;
		return *this;

	}

	FORCEINLINE UKRVCharacterStatComponent& operator*=(const AKRVBuff& Other)
	{
		*(Cast<UKRVStatComponent>(this)) *= Other;
		MeleeAttackStat *= Other.AttackStat;
		DefenceStat *= Other.DefenceStat;
		MovementSpeed *= (1 + Other.MovementSpeedScale);
		return *this;

	}

};
