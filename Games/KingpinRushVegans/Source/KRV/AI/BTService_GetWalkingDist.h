// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "BTService_GetWalkingDist.generated.h"

UCLASS()
class KRV_API UBTService_GetWalkingDist : public UBTService
{
	GENERATED_BODY()

public:
	UBTService_GetWalkingDist();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:
	UPROPERTY()
		class AKRVCharacter* KRVRunner;

	TArray<int32> RecendDists;
	int32 TicksToCheck = 10;
	int32 CurrentIndex;
	FVector RecentLocation;
	float SumOfRecentDists;

};
