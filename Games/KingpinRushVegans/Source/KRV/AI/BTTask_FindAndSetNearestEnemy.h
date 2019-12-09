// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindAndSetNearestEnemy.generated.h"

UCLASS()
class KRV_API UBTTask_FindAndSetNearestEnemy : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindAndSetNearestEnemy();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	
};
