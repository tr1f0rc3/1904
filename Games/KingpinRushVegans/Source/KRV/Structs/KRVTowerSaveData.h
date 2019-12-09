// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVTowerSaveData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVTowerSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		FName TowerName;

	UPROPERTY(BlueprintReadWrite)
		bool bIsUnlocked = true;

	UPROPERTY(BlueprintReadWrite)
		bool bIsRecentlyUnlocked = true;

};
