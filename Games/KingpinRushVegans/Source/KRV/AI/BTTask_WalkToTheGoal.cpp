// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_WalkToTheGoal.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "Actors/KRVPath.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVEnemyAIController.h"

UBTTask_WalkToTheGoal::UBTTask_WalkToTheGoal()
{
	bNotifyTick = true;
	NodeName = TEXT("WalkToTheGoal");

}

EBTNodeResult::Type UBTTask_WalkToTheGoal::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!KRVCharacter)
	{
		KRVCharacter = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	}
	if (!PathToFollow)
	{
		PathToFollow = Cast<AKRVPath>(OwnerComp.GetBlackboardComponent()->
			GetValueAsObject(AKRVEnemyAIController::PathToFollowKey));
	}

	if (!KRVCharacter || !PathToFollow)
	{
		KRVCHECK(KRVCharacter);
		KRVCHECK(PathToFollow);
	}
	else if (KRVCharacter->IsAbleToMove())
	{
		if (PathToFollow->GetPathLocationAtTime(/*OutValue*/GoalLocation, 1.f))
		{
			if (KRVCharacter->SetFollowingPathStatus() && KRVCharacter->FollowPath(GoalLocation))
			{
				UAIBlueprintHelperLibrary::SimpleMoveToLocation(OwnerComp.GetAIOwner(), GoalLocation);
				return EBTNodeResult::Succeeded;
			}
		}
	}
	return EBTNodeResult::Failed;

}

