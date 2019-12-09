// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DataAssets/KRVDataAsset.h"
#include "Structs/KRVCharacterSpawnData.h"
#include "KRVGatewayDataAsset.generated.h"

UCLASS()
class KRV_API UKRVGatewayDataAsset : public UKRVDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
		TArray<FKRVCharacterSpawnData> CharacterSpawnData;

};
