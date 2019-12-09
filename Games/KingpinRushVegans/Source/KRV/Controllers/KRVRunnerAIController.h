// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/KRVAIController.h"
#include "KRVRunnerAIController.generated.h"

UCLASS()
class KRV_API AKRVRunnerAIController : public AKRVAIController
{
	GENERATED_BODY()

public:
	AKRVRunnerAIController();

public:
	virtual void OnPossess(APawn* aPawn) override;

public: // Keys
	static const FName PathToFollowKey;
	static const FName SplineTimeKey;
	static const FName IsFlyingKey;
	static const FName WalkingDistKey;

};
