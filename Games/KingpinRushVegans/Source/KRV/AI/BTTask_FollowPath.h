// Fill out your copyright notice in the Description page of Project Settings.
// reference (kr): http://alleysark.kr/how-to-make-spline-following-ai-with-behavior-tree/

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_FollowPath.generated.h"

UCLASS()
class KRV_API UBTTask_FollowPath : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_FollowPath();

public:
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;
	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;


protected:
	bool IsOpenLoopAndAlreadyAtGoal(class AKRVPath* Path, float CurrentTime, float Precision = 0.01f) const;

protected:
	UPROPERTY()
		class AKRVCharacter* KRVCharacter;

	UPROPERTY()
		class AKRVPath* PathToFollow;

	float TimePrecision = 0.01f;

};
