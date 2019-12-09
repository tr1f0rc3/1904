// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "Interfaces/KRVSkillInterface.h"
#include "KRVItemDataAsset.generated.h"

UCLASS()
class KRV_API UKRVItemDataAsset : public UKRVDataAsset, public IKRVSkillInterface
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stock")
		int32 MaxStocks = 5;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Price")
		int32 Price = 0;

};
