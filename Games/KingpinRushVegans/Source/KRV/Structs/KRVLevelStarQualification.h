// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVLevelStarQualification.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVLevelStarQualification
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 StarsToEarn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		float ClearTime = -1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
		int32 HeartsRemaining = 16;

};
