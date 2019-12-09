// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_TurnToTarget.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "ActorComponents/KRVCharacterStatComponent.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVPlayerAIController.h"

UBTTask_TurnToTarget::UBTTask_TurnToTarget()
{
	NodeName = TEXT("TurnToTarget");
	bNotifyTick = true;

}

EBTNodeResult::Type UBTTask_TurnToTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCombatUnit* PawnToTurn = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());
	if (!PawnToTurn)
	{
		KRVCHECK(PawnToTurn);
		return EBTNodeResult::Failed;
	}
	else
	{
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;

}

void UBTTask_TurnToTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AKRVCombatUnit* PawnToTurn = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());
	if (!PawnToTurn)
	{
		KRVCHECK(PawnToTurn);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!PawnToTurn->GetTargetUnit() || PawnToTurn->GetActionStatus() == EActionStatus::Dead)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (PawnToTurn->GetActionStatus() == EActionStatus::Stunned)
	{
		// InProgress
	}
	else
	{
		FVector ForwardVectorTo = (PawnToTurn->GetTargetUnit()->GetActorLocation() -
			PawnToTurn->GetActorLocation()).GetSafeNormal2D();
		ForwardVectorTo = ForwardVectorTo.GetSafeNormal2D();

		FVector CurrentForwardVector = PawnToTurn->GetActorForwardVector().GetSafeNormal2D();

		if (FVector::DotProduct(CurrentForwardVector, ForwardVectorTo) > 0.95f)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
		else
		{
			FRotator RotationToTurn = FRotationMatrix::MakeFromX(ForwardVectorTo).Rotator();
			PawnToTurn->SetActorRotation(FMath::RInterpTo
			(PawnToTurn->GetActorRotation(), RotationToTurn, DeltaSeconds, 10.0f));
		}
	}

}
