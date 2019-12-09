// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVCommanderTalentData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVCommanderTalentData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "ID")
		FName TalentName;

	UPROPERTY(BlueprintReadWrite, Category = "Unlock")
		bool bIsUnlocked = false;

	UPROPERTY(BlueprintReadWrite, Category = "Exp")
		int32 Level = 1;

	UPROPERTY(BlueprintReadWrite, Category = "Exp")
		int32 CurrentLevelExp = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		TArray<int32> SkillMasteryLevels;

	UPROPERTY(BlueprintReadWrite, Category = "Skill")
		int32 EarnedSkillPoints = 0;

};