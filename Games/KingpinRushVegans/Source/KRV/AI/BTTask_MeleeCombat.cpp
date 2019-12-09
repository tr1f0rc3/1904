// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_MeleeCombat.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "ActorComponents/KRVStatComponent.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVPlayerAIController.h"

UBTTask_MeleeCombat::UBTTask_MeleeCombat()
{
	bNotifyTick = true;
	NodeName = TEXT("Melee");

}

EBTNodeResult::Type UBTTask_MeleeCombat::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCharacter* Combatant = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());

	if (!Combatant)
	{
		KRVCHECK(Combatant);
	}
	else if (!Combatant->IsAbleToMeleeAttack())
	{
		if (Combatant->GetActionStatus() == EActionStatus::Attacking &&
			Combatant->GetCombatStatus() == ECombatStatus::Melee)
		{
			bDidAttackStart = true;
			return EBTNodeResult::InProgress;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}
	else if (Combatant->GetTargetUnit() && Combatant->IsInChargeRange(Cast<AKRVCharacter>(Combatant->GetTargetUnit())))
	{
		if (!(Combatant->GetCombatStatus() == ECombatStatus::Melee &&
			Combatant->GetActionStatus() == EActionStatus::Moving))
		{
			Combatant->StopMoving();
		}

		bDidAttackStart = false;
		bDidChargeStart = false;
		return EBTNodeResult::InProgress;
	}
	else
	{
		AKRVCharacter* CharacterToMelee = Cast<AKRVCharacter>
			(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AKRVAIController::UnitToCheckKey));

		if (CharacterToMelee && Combatant->IsInChargeRange(CharacterToMelee))
		{
			Combatant->SetTargetUnit(CharacterToMelee);
			Combatant->StopMoving();
			bDidAttackStart = false;
			bDidChargeStart = false;
			return EBTNodeResult::InProgress;
		}
	}
	return EBTNodeResult::Failed;

}

void UBTTask_MeleeCombat::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AIController = OwnerComp.GetAIOwner();
	AKRVCharacter* Combatant = Cast<AKRVCharacter>(AIController->GetPawn());

	if (!Combatant)
	{
		KRVCHECK(Combatant);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!Combatant->GetTargetUnit())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!Combatant->IsAbleToMove())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (!Combatant->IsInChargeRange())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (bDidAttackStart)
	{
		if (Combatant->GetCombatStatus() == ECombatStatus::Melee)
		{
			if (Combatant->GetActionStatus() == EActionStatus::Attacking)
			{
				// InProgress, after attack started
			}
			else if (Combatant->GetActionStatus() == EActionStatus::Idle)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else if (bDidChargeStart)
	{
		if (Combatant->GetCombatStatus() == ECombatStatus::Melee &&
			Combatant->GetActionStatus() == EActionStatus::Attacking)
		{
			bDidAttackStart = true;
			bDidChargeStart = false;
		}
		else if (Combatant->GetActionStatus() == EActionStatus::Moving)
		{
			// not implemented;
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else if (!bDidAttackStart && !bDidChargeStart)
	{
		if (Combatant->MeleeAttack())
		{
			bDidAttackStart = true;
		}
		else if (Combatant->ChargeToMelee())
		{
			bDidChargeStart = true;
		}
		else
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}

}
