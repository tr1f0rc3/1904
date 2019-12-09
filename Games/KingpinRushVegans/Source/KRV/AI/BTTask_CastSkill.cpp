// Fill out your copyright notice in the Description page of Project Settings.


#include "BTTask_CastSkill.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Actors/KRVSkill.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVAIController.h"

UBTTask_CastSkill::UBTTask_CastSkill()
{
	bNotifyTick = true;
	NodeName = TEXT("CastSkill");

}

EBTNodeResult::Type UBTTask_CastSkill::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCombatUnit* KRVCaster = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());

	if (!KRVCaster)
	{
		KRVCHECK(KRVCaster);
		return EBTNodeResult::Failed;
	}
	else if (!KRVCaster->IsAbleToAttack())
	{
		return EBTNodeResult::Failed;
	}
	else
	{
		bool bDidSkillCast = false;
		AKRVSkill* SkillToCast = KRVCaster->GetSkillByTag(KRVCaster->GetReservedSkillTag());
		if (SkillToCast)
		{
			ETargetingType TargetingType = SkillToCast->GetTargetingType();
			switch (TargetingType)
			{
			case ETargetingType::Self:
			{
				bDidSkillCast = KRVCaster->CastSkill(KRVCaster->GetReservedSkillTag());
				break;
			}
			case ETargetingType::Target:
			{
				AKRVCombatUnit* UnitToCast = Cast<AKRVCombatUnit>
					(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AKRVAIController::UnitToCheckKey));
				if (UnitToCast)
				{
					bDidSkillCast = KRVCaster->CastSkill(KRVCaster->GetReservedSkillTag(), UnitToCast);
				}
				break;
			}
			case ETargetingType::Location:
			{
				FVector LocationToCast =
					(OwnerComp.GetBlackboardComponent()->GetValueAsVector(AKRVAIController::PointToCheckKey));
				if (LocationToCast != FVector::ZeroVector)
				{
					bDidSkillCast = KRVCaster->CastSkill(KRVCaster->GetReservedSkillTag(), nullptr, LocationToCast);
				}
				break;
			}
			default:
				break;
			}
		}

		if (bDidSkillCast)
		{
			return EBTNodeResult::InProgress;
		}
		else
		{
			return EBTNodeResult::Failed;
		}
	}

}

void UBTTask_CastSkill::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	auto AIController = OwnerComp.GetAIOwner();
	AKRVCombatUnit* KRVCaster = Cast<AKRVCombatUnit>(AIController->GetPawn());

	if (!KRVCaster)
	{
		KRVCHECK(KRVCaster);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (KRVCaster->GetActionStatus() == EActionStatus::Casting)
	{
		// InProgress
	}
	else if (!KRVCaster->IsAbleToMove())
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
	}

}
