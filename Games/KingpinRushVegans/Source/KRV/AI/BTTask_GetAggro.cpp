// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_GetAggro.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "ActorComponents/KRVCharacterStatComponent.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVPlayerAIController.h"

UBTTask_GetAggro::UBTTask_GetAggro()
{
	bNotifyTick = true;
	NodeName = TEXT("GetAggro");

}

EBTNodeResult::Type UBTTask_GetAggro::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCharacter* Combatant = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!Combatant)
	{
		KRVCHECK(Combatant);
		return EBTNodeResult::Failed;
	}
	else if (Combatant->GetAggro())
	{
		return EBTNodeResult::InProgress;
	}
	return EBTNodeResult::Failed;

}

void UBTTask_GetAggro::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AIController = OwnerComp.GetAIOwner();
	AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(AIController->GetPawn());

	if (!KRVCharacter)
	{
		KRVCHECK(KRVCharacter);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!KRVCharacter->IsAbleToMove())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (KRVCharacter->GetActionStatus() == EActionStatus::Moving)
	{
		// InProgress
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
