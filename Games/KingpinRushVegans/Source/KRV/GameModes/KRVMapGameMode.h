// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "KRVMapGameMode.generated.h"

/**
 * 
 */
UCLASS()
class KRV_API AKRVMapGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AKRVMapGameMode();

public:
	virtual void PostLogin(APlayerController* NewPlayer) override;

public:
	int32 GetAvaialbleSkillPointsFromHeroLevel(int32 NewHeroLevel) const;

};
