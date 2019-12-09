// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVSkillInterface.generated.h"

class AKRVCombatUnit;

UINTERFACE(MinimalAPI)
class UKRVSkillInterface : public UInterface
{
	GENERATED_BODY()

};

class KRV_API IKRVSkillInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool CastToSelf(AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool CastToTarget(AKRVCombatUnit* Target, AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool CastToLocation(const FVector& Location, AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Skill")
		bool IsAbleToCastSkill(AKRVCombatUnit* NewCaster = nullptr);

};