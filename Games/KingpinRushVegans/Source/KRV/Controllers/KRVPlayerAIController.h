// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/KRVAIController.h"
#include "KRVPlayerAIController.generated.h"

UCLASS()
class KRV_API AKRVPlayerAIController : public AKRVAIController
{
	GENERATED_BODY()

public:
	AKRVPlayerAIController();

public:
	virtual void OnPossess(APawn* InPawn) override;

public:
	static const FName IsRecruitedByTowerKey;

};
