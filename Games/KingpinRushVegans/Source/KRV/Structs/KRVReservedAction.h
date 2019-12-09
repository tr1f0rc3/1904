// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVEnums.h"
#include "GameplayTagContainer.h"
#include "KRVReservedAction.generated.h"

USTRUCT(BlueprintType)
struct FKRVReservedAction
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EActionStatus ActionStatus = EActionStatus::NONE;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ECombatStatus CombatStatus;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TWeakObjectPtr<class AKRVCombatUnit> TargetUnit = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FVector TargetPoint = FVector::ZeroVector;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FGameplayTag SkillTagToCast;

};
