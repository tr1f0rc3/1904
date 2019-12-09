// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MeleeCombat.generated.h"

UCLASS()
class KRV_API UBTTask_MeleeCombat : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MeleeCombat();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	bool bDidAttackStart = false;
	bool bDidChargeStart = false;

};
