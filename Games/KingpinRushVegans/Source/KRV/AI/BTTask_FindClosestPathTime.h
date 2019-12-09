// Fill out your copyright notice in the Description page of Project Settings.
// reference (kr): http://alleysark.kr/how-to-make-spline-following-ai-with-behavior-tree/

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FindClosestPathTime.generated.h"

UCLASS()
class KRV_API UBTTask_FindClosestPathTime : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FindClosestPathTime();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

};
