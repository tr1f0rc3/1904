// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_FindAndSetNearestEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVAIController.h"

UBTTask_FindAndSetNearestEnemy::UBTTask_FindAndSetNearestEnemy()
{
	NodeName = TEXT("FindAndSetNearestEnemy");

}

EBTNodeResult::Type UBTTask_FindAndSetNearestEnemy::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCombatUnit* UnitOnGuard = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());
	AKRVCharacter* CharacterOnGuard = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	
	if (!UnitOnGuard)
	{
		KRVCHECK(UnitOnGuard);
		return EBTNodeResult::Failed;
	}
	else if (UnitOnGuard->GetActionStatus() == EActionStatus::Dead)
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		AKRVCharacter* NearestEnemy = nullptr;
		if (CharacterOnGuard)
		{
			ECombatStatus FoundRange = CharacterOnGuard->FindNearestEnemyRange(NearestEnemy, ECombatStatus::Melee);
		}
		else
		{
			ECombatStatus FoundRange = UnitOnGuard->FindNearestEnemyRange(NearestEnemy, ECombatStatus::Melee);
		}

		if (!NearestEnemy)
		{
			return EBTNodeResult::Failed;
		}
		else
		{
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(AKRVAIController::UnitToCheckKey, NearestEnemy);
			return EBTNodeResult::Succeeded;
		}
	}

}
