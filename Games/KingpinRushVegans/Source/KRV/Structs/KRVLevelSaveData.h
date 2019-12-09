// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVLevelSaveData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVLevelSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "ID")
		FString LevelString;

	UPROPERTY(BlueprintReadWrite, Category = "Unlock")
		bool bIsUnlocked = false;

	UPROPERTY(BlueprintReadWrite, Category = "Unlock")
		bool bIsRecentlyUnlocked = true;

	UPROPERTY(BlueprintReadWrite, Category = "Star")
		int32 EarnedStars = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Star")
		bool bIsRecentlyCleared = false;

	UPROPERTY(BlueprintReadWrite, Category = "Time")
		float BestClearTime = 99999.f;

	FORCEINLINE void operator=(const FKRVLevelSaveData& Other)
	{
		LevelString = Other.LevelString;
		bIsUnlocked = Other.bIsUnlocked;
		bIsRecentlyUnlocked = Other.bIsRecentlyUnlocked;
		EarnedStars = Other.EarnedStars;
		BestClearTime = Other.BestClearTime;
	}

};
