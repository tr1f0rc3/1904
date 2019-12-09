// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVItemStockData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVItemStockData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FName ItemName;

	UPROPERTY(BlueprintReadWrite)
		bool bIsUnlocked = true;

	UPROPERTY(BlueprintReadWrite)
		bool bIsRecentlyUnlocked = true;

	UPROPERTY(BlueprintReadWrite)
		int32 Stock = 0;

};

