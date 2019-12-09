// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVSkillMasterySaveData.h"
#include "KRVHeroSaveData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVHeroSaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "ID")
		FName HeroName = "BAKI";

	UPROPERTY(BlueprintReadWrite, Category = "Unlock")
		bool bIsUnlocked = true;

	UPROPERTY(BlueprintReadWrite, Category = "Unlock")
		bool bIsRecentlyUpdated = true;

	UPROPERTY(BlueprintReadWrite, Category = "Exp")
		int32 Level = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Exp")
		int32 CurrentLevelExp = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		TArray<FKRVSkillMasterySaveData> SkillMasterySaveData;

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		int32 SkillPointsGiven = 2;

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		int32 RemainingSkillPoints = 2;

};