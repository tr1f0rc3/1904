// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_FollowPath.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "Actors/KRVPath.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVRunnerAIController.h"

UBTTask_FollowPath::UBTTask_FollowPath()
{
	bNotifyTick = true;
	NodeName = TEXT("FollowPath");

}

EBTNodeResult::Type UBTTask_FollowPath::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	if (!KRVCharacter)
	{
		KRVCharacter = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	}
	if (!PathToFollow)
	{
		PathToFollow = Cast<AKRVPath>(OwnerComp.GetBlackboardComponent()->
			GetValueAsObject(AKRVRunnerAIController::PathToFollowKey));
	}

	if (!KRVCharacter || !PathToFollow)
	{
		KRVCHECK(KRVCharacter);
		KRVCHECK(PathToFollow);
	}
	else if (KRVCharacter->SetFollowingPathStatus())
	{
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;

}

void UBTTask_FollowPath::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AIController = OwnerComp.GetAIOwner();
	if (!KRVCharacter)
	{
		KRVCharacter = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	}
	if (!PathToFollow)
	{
		PathToFollow = Cast<AKRVPath>(OwnerComp.GetBlackboardComponent()->
			GetValueAsObject(AKRVRunnerAIController::PathToFollowKey));
	}

	if (!KRVCharacter || !PathToFollow)
	{
		KRVCHECK(KRVCharacter);
		KRVCHECK(PathToFollow);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (KRVCharacter->IsAbleToMove())
	{
		float CurrentTime = OwnerComp.GetBlackboardComponent()->
			GetValueAsFloat(AKRVRunnerAIController::SplineTimeKey);
		FVector PathLocationAtTime;
		if (PathToFollow->GetPathLocationAtTime(/*OutValue*/PathLocationAtTime, CurrentTime))
		{
			float UpdatedTime = CurrentTime;
			while (!KRVCharacter->FollowPath(PathLocationAtTime))
			{
				if (IsOpenLoopAndAlreadyAtGoal(PathToFollow, UpdatedTime, TimePrecision))
				{
					break;
				}
				UpdatedTime += TimePrecision;
			}
			UpdatedTime = PathToFollow->ComputeTimeSepartedFrom
			(CurrentTime, KRVCharacter->GetActorLocation());
			if (UpdatedTime == -1.f)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			}
			else if (IsOpenLoopAndAlreadyAtGoal(PathToFollow, UpdatedTime, TimePrecision))
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			else
			{
				OwnerComp.GetBlackboardComponent()->SetValueAsFloat
				(AKRVRunnerAIController::SplineTimeKey, UpdatedTime);
			}
		}
	}
	FinishLatentTask(OwnerComp, EBTNodeResult::Failed);

}

void UBTTask_FollowPath::OnTaskFinished(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	KRVCharacter = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (KRVCharacter)
	{
		KRVCharacter->SetFollowingPathStatus(false);
	}

}

bool UBTTask_FollowPath::IsOpenLoopAndAlreadyAtGoal(AKRVPath* Path, float CurrentTime, float Precision) const
{
	if (!Path || Path->IsPathClosedLoop())
	{
		return false;
	}
	else if (FMath::IsNearlyEqual(CurrentTime, Path->GetPathDuration(), Precision))
	{
		return true;
	}
	return false;

}
