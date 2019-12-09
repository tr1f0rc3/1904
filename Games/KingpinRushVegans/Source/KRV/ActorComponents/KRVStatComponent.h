// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "Structs/KRVAttackStat.h"
#include "Actors/KRVBuff.h"
#include "KRVStatComponent.generated.h"

// Components store values of current 
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KRV_API UKRVStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UKRVStatComponent();

public:
	UFUNCTION(BlueprintCallable, Category = "Load")
		virtual UKRVStatComponent* LoadData(const FName& UnitName);

public: // Ranged
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
		FKRVAttackStat RangedAttackStat;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
		float Sight;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ranged")
		ERangedMediumType RangedMediumType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Splash")
		FKRVAttackStat SplashAttackStat;


	UKRVStatComponent* AssignmentOperator(const UKRVStatComponent* const Other);

	FORCEINLINE UKRVStatComponent& operator+=(const AKRVBuff& Other)
	{
		RangedAttackStat += Other.AttackStat;
		return *this;

	}

	FORCEINLINE UKRVStatComponent& operator*=(const AKRVBuff& Other)
	{
		RangedAttackStat *= Other.AttackStat;
		return *this;

	}

};
