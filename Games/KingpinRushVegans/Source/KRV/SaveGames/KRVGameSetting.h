// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "KRVGameSetting.generated.h"

UCLASS()
class KRV_API UKRVGameSetting : public USaveGame
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadWrite)
		float SoundVolume = 0.5f;

};
