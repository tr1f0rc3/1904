// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_Fire.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"

#include "ActorComponents/KRVStatComponent.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVAIController.h"

UBTTask_Fire::UBTTask_Fire()
{
	bNotifyTick = true;
	NodeName = TEXT("Fire");

}

EBTNodeResult::Type UBTTask_Fire::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCombatUnit* KRVShooter = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());
	AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(KRVShooter);

	if (!KRVShooter)
	{
		KRVCHECK(KRVShooter);
		return EBTNodeResult::Failed;
	}
	else if (!KRVShooter->GetBaseStat() &&
		KRVShooter->GetBaseStat()->RangedMediumType == ERangedMediumType::NoWeapon)
	{
		KRVCHECK(KRVShooter->GetBaseStat());
		return EBTNodeResult::Failed;
	}
	else if (!KRVShooter->IsAbleToAttack())
	{
		if (KRVShooter->GetActionStatus() == EActionStatus::Attacking &&
			KRVShooter->GetCombatStatus() == ECombatStatus::Ranged)
		{
			bDidAttackStart = true;
			return EBTNodeResult::InProgress;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
	else if (KRVShooter->GetTargetUnit() && KRVShooter->IsInFireRange())
	{
		if (KRVCharacter)
		{
			KRVCharacter->StopMoving();
		}
		bDidAttackStart = false;
		return EBTNodeResult::InProgress;
	}
	else
	{
		AKRVCombatUnit* UnitToShoot = Cast<AKRVCombatUnit>
			(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AKRVAIController::UnitToCheckKey));
		if (UnitToShoot && KRVShooter->IsInFireRange(UnitToShoot))
		{
			if (KRVCharacter)
			{
				KRVCharacter->StopMoving();
			}
			KRVShooter->SetTargetUnit(UnitToShoot);
			bDidAttackStart = false;
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;

}

void UBTTask_Fire::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	auto AIController = OwnerComp.GetAIOwner();
	AKRVCombatUnit* KRVShooter = Cast<AKRVCombatUnit>(AIController->GetPawn());

	if (!KRVShooter)
	{
		KRVCHECK(KRVShooter);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!KRVShooter->IsAbleToMove())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!KRVShooter->IsInFireRange())
	{
		KRVCHECK(KRVShooter);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!KRVShooter->IsAbleToAttack())
	{
		// InProgress, before attack started
	}
	else if (bDidAttackStart)
	{
		if (KRVShooter->GetActionStatus() == EActionStatus::Attacking)
		{
			// InProgress, after attack started
		}
		else if (KRVShooter->GetActionStatus() == EActionStatus::Idle)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		}
	}
	else if (!bDidAttackStart && KRVShooter->Fire())
	{
		bDidAttackStart = true; // InProgress
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

}
