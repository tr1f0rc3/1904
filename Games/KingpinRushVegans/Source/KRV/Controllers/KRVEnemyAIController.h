// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/KRVRunnerAIController.h"
#include "KRVEnemyAIController.generated.h"

UCLASS()
class KRV_API AKRVEnemyAIController : public AKRVRunnerAIController
{
	GENERATED_BODY()

public:
	AKRVEnemyAIController();
	
public:
	virtual void OnPossess(APawn* aPawn) override;
	
};
