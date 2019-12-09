// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Controllers/KRVAIController.h"
#include "KRVCharacterAIController.generated.h"

UCLASS(Abstract)
class KRV_API AKRVCharacterAIController : public AKRVAIController
{
	GENERATED_BODY()

public:
	AKRVCharacterAIController();

public:
	virtual void OnPossess(APawn* aPawn) override;

public:
	UFUNCTION(BlueprintCallable)
		class AKRVCharacter* GetCharacter() const;
	
};
