// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVCharacterDataAsset.h"
#include "KRVHeroDataAsset.generated.h"

UCLASS()
class KRV_API UKRVHeroDataAsset : public UKRVCharacterDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Revive")
		float ReviveTime = 20.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Exp")
		TArray<int32> ExpToLevelUp;

};
