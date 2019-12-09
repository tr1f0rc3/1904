// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVPlayerResourceData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVPlayerResourceData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		int32 CommanderPoints = 0;

	UPROPERTY(BlueprintReadWrite)
		int32 Ruby = 50;

	FORCEINLINE void operator=(const FKRVPlayerResourceData& Other)
	{
		CommanderPoints = Other.CommanderPoints;
		Ruby = Other.Ruby;

	}

};

