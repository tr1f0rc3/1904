// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorComponents/KRVStatComponent.h"
#include "Actors/KRVBuff.h"
#include "KRVTowerStatComponent.generated.h"


UCLASS()
class KRV_API UKRVTowerStatComponent : public UKRVStatComponent
{
	GENERATED_BODY()
	

public:
	virtual UKRVStatComponent* LoadData(const FName& UnitName) override;

public: // loaded variables
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		int32 WaypointRadius = 0.f;

	UKRVTowerStatComponent* AssignmentOperator(const UKRVTowerStatComponent* const Other);

	FORCEINLINE UKRVTowerStatComponent& operator+=(const AKRVBuff& Other)
	{
		*(Cast<UKRVStatComponent>(this)) += Other;
		SplashAttackStat += Other.AttackStat;
		return *this;

	}

	FORCEINLINE UKRVTowerStatComponent& operator*=(const AKRVBuff& Other)
	{
		*(Cast<UKRVStatComponent>(this)) *= Other;
		SplashAttackStat *= Other.AttackStat;
		return *this;

	}
	

};
