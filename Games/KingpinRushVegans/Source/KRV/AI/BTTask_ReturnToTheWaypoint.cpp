// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_ReturnToTheWaypoint.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Pawns/KRVTower.h"
#include "Controllers/KRVPlayerAIController.h"

UBTTask_ReturnToTheWaypoint::UBTTask_ReturnToTheWaypoint()
{
	bNotifyTick = true;
	NodeName = TEXT("GoBackToTheWaypoint");

}

EBTNodeResult::Type UBTTask_ReturnToTheWaypoint::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCharacter* Combatant = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Combatant)
	{
		KRVCHECK(Combatant);
		return EBTNodeResult::Failed;
	}
	else if (!Combatant->IsAbleToAttack() || !Combatant->bIsRecruitedByTower)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		AKRVTower* RecruiterTower = Cast<AKRVTower>(Combatant->GetSpawnerOfThis());
		if (!RecruiterTower)
		{
			KRVCHECK(RecruiterTower);
			return EBTNodeResult::Failed;
		}
		else if (Combatant->GetCombatStatus() == ECombatStatus::FollowingPath)
		{
			return EBTNodeResult::InProgress;
		}
		else
		{
			RecruiterTower->MakeRecruitsReturnToTheWaypoint();
			if (Combatant->GetCombatStatus() == ECombatStatus::FollowingPath)
			{
				return EBTNodeResult::InProgress;
			}
		}
	}
	return EBTNodeResult::Failed;

}

void UBTTask_ReturnToTheWaypoint::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AKRVCharacter* Combatant = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (Combatant->GetCombatStatus() == ECombatStatus::FollowingPath)
	{
		// InProgress
	}
	else if (Combatant->GetCombatStatus() == ECombatStatus::Idle)
	{
		// It may not be actually succeeded.
		// 실제로 성공이 아닐수도 있음.
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded); 
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

}
