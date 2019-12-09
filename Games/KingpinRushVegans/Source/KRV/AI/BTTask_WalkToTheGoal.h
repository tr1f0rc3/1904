// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_WalkToTheGoal.generated.h"

UCLASS()
class KRV_API UBTTask_WalkToTheGoal : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_WalkToTheGoal();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	UPROPERTY()
		class AKRVCharacter* KRVCharacter;

	UPROPERTY()
		class AKRVPath* PathToFollow;

	UPROPERTY()
		FVector GoalLocation;

};
