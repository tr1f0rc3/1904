// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KRVEnums.h"
#include "KRVSkillMasterySaveData.generated.h"

USTRUCT(Atomic, BlueprintType)
struct FKRVSkillMasterySaveData
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite, Category = "ID")
		FName SkillName;

	UPROPERTY(BlueprintReadWrite, Category = "Exp")
		int32 MasteryLevel = 0;

	UPROPERTY(BlueprintReadWrite, Category = "Exp")
		int32 MaxMasteryLevel = MaxSkillMasteryLevel;

};