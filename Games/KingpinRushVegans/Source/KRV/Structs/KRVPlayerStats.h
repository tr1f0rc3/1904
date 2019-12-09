// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVPlayerStats.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVPlayerStats
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		int32 EarnedStars = 0;

	UPROPERTY(BlueprintReadWrite)
		int32 Kills = 0;

	UPROPERTY(BlueprintReadWrite)
		int32 EarnedGold = 0;

	UPROPERTY(BlueprintReadWrite)
		int32 SpentGold = 0;

	UPROPERTY(BlueprintReadWrite)
		int32 GainedExp = 0;

	UPROPERTY(BlueprintReadWrite)
		int32 EarnedRuby = 0;

	UPROPERTY(BlueprintReadWrite)
		int32 Recruits = 0;

	FORCEINLINE void operator=(const FKRVPlayerStats& Other)
	{
		EarnedStars = Other.EarnedStars;
		Kills = Other.Kills;
		EarnedGold = Other.EarnedGold;
		SpentGold = Other.SpentGold;
		GainedExp = Other.GainedExp;
		EarnedRuby = Other.EarnedRuby;
		Recruits = Other.Recruits;
	}

};

