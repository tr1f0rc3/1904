// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindClosestPathTime.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"

#include "Actors/KRVPath.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVRunnerAIController.h"

UBTTask_FindClosestPathTime::UBTTask_FindClosestPathTime()
{
	NodeName = TEXT("FindClosestPathTime");

}

EBTNodeResult::Type UBTTask_FindClosestPathTime::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	auto ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn)
	{
		KRVCHECK(ControllingPawn);
		return EBTNodeResult::Failed;
	}
	else
	{
		AKRVPath* PathToFollow = Cast<AKRVPath>(OwnerComp.GetBlackboardComponent()->
			GetValueAsObject(AKRVRunnerAIController::PathToFollowKey));
		if (!PathToFollow)
		{
			KRVCHECK(PathToFollow);
			return EBTNodeResult::Failed;
		}
		else
		{
			float ClosestTime = PathToFollow->FindClosestTimeToWorldLocation
			(ControllingPawn->GetActorLocation());
			if (ClosestTime == -1.f)
			{
				return EBTNodeResult::Failed;
			}
			else
			{
				AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(ControllingPawn);
				if (KRVCharacter)
				{
					KRVCharacter->SplineTime = ClosestTime;
				}

				float UpdatedTime = PathToFollow->ComputeTimeSepartedFrom
				(ClosestTime, ControllingPawn->GetActorLocation());
				if (UpdatedTime == -1.f)
				{
					return EBTNodeResult::Failed;
				}
				else
				{
					OwnerComp.GetBlackboardComponent()->SetValueAsFloat
					(AKRVRunnerAIController::SplineTimeKey, UpdatedTime);
					return EBTNodeResult::Succeeded;
				}
			}
		}
	}

}
