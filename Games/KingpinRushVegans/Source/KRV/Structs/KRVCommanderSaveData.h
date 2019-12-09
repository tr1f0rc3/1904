// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVCommanderSaveData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVCommanderSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		TArray<FName> AcquiredTalents;

	UPROPERTY(BlueprintReadWrite)
		int32 RemainingTalentPoints;

	UPROPERTY(BlueprintReadWrite)
		TArray<int32> SkillMasteryLevels;

};