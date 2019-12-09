// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "KRV.h"
#include "Actors/KRVSkill.h"
#include "ActorComponents/KRVStatComponent.h"
#include "Controllers/KRVInGamePlayerController.h"
#include "Pawns/KRVCombatUnit.h"

const FName AKRVAIController::SelfIDKey(TEXT("SelfID"));
const FName AKRVAIController::HasActiveSkillKey(TEXT("HasActiveSkill"));
const FName AKRVAIController::RangedMediumTypeKey(TEXT("RangedMediumType"));
const FName AKRVAIController::ActionStatusKey(TEXT("ActionStatus"));
const FName AKRVAIController::CombatStatusKey(TEXT("CombatStatus"));
const FName AKRVAIController::NearestEnemyRangeKey(TEXT("NearestEnemyRange"));
const FName AKRVAIController::TargetUnitKey(TEXT("TargetUnit"));
const FName AKRVAIController::TargetUnitIDKey(TEXT("TargetUnitID"));
const FName AKRVAIController::TargetPointKey(TEXT("TargetPoint"));
const FName AKRVAIController::UnitToCheckKey(TEXT("UnitToCheck"));
const FName AKRVAIController::PointToCheckKey(TEXT("PointToCheck"));


AKRVAIController::AKRVAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Resources/AI/BB_CombatUnit.BB_CombatUnit"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Resources/AI/BT_CombatUnit.BT_CombatUnit"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void AKRVAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (UseBlackboard(BBAsset, Blackboard) && GetKRVCombatUnit())
	{
		InitBBProperties();
		GetKRVCombatUnit()->OnSkillAcquireDelegate.AddUObject(this, &AKRVAIController::OnSkillAcuqired);
		GetKRVCombatUnit()->OnActionStatusChangedDelegate.AddUObject(this, &AKRVAIController::OnActionStatusChanged);
		GetKRVCombatUnit()->OnCombatStatusChangedDelegate.AddUObject(this, &AKRVAIController::OnCombatStatusChanged);
		GetKRVCombatUnit()->OnNearestEnemyRangeChangedDelegate.AddUObject(this, &AKRVAIController::OnNearestEnemyRangeKeyChanged);
		GetKRVCombatUnit()->OnTargetUnitChangedDelegate.AddUObject(this, &AKRVAIController::OnTargetUnitChanged);
		GetKRVCombatUnit()->OnTargetPointChangedDelegate.AddUObject(this, &AKRVAIController::OnTargetPointChanged);

		if (!RunBehaviorTree((BTAsset)))
		{
			KRVLOG(Error, TEXT("Couldn't run BT"));
		}
	}

}

AKRVCombatUnit* AKRVAIController::GetKRVCombatUnit()
{
	if (!KRVCombatUnit)
	{
		KRVCombatUnit = Cast<AKRVCombatUnit>(GetPawn());
	}
	return KRVCombatUnit;

}

AKRVInGamePlayerController* AKRVAIController::GetPlayerControllerByTeam()
{
	if (!GetWorld() || !GetKRVCombatUnit())
	{
		KRVCHECK(GetWorld());
		KRVCHECK(GetKRVCombatUnit());
		return nullptr;
	}
	else
	{
		switch (GetKRVCombatUnit()->GetTeam())
		{
		case EKRVTeam::Player:
		{
			if (GetGameInstance() && GetGameInstance()->GetEngine())
			{
				AKRVInGamePlayerController* KRVInGamePlayerController = Cast<AKRVInGamePlayerController>
					(GetGameInstance()->GetEngine()->GetFirstLocalPlayerController(GetWorld()));
				return KRVInGamePlayerController;
			}
			break;
		}
		default:
			break;
		}

	}
	return nullptr;

}

void AKRVAIController::InitBBProperties()
{
	KRVCHECK(GetKRVCombatUnit());
	if (UseBlackboard(BBAsset, Blackboard) && GetKRVCombatUnit())
	{
		Blackboard->SetValueAsInt(SelfIDKey, GetKRVCombatUnit()->GetUnitInFieldID());
		Blackboard->SetValueAsEnum(RangedMediumTypeKey, uint8(GetKRVCombatUnit()->GetBaseStat()->RangedMediumType));
		Blackboard->SetValueAsBool(HasActiveSkillKey, GetKRVCombatUnit()->HasActiveSkill());
		Blackboard->SetValueAsBool(HasActiveSkillKey, GetKRVCombatUnit()->HasActiveSkill());
		Blackboard->SetValueAsEnum(CombatStatusKey, uint8(GetKRVCombatUnit()->GetCombatStatus()));
		Blackboard->SetValueAsObject(TargetUnitKey, GetKRVCombatUnit()->GetTargetUnit());
		if (KRVCombatUnit->GetTargetUnit())
		{
			Blackboard->SetValueAsInt(TargetUnitKey, GetKRVCombatUnit()->GetTargetUnit()->GetUnitInFieldID());
		}
		else
		{
			Blackboard->SetValueAsInt(TargetUnitKey, -1);
		}
		Blackboard->SetValueAsVector(TargetPointKey, GetKRVCombatUnit()->GetTargetPoint());
		Blackboard->SetValueAsEnum(CombatStatusKey, uint8(GetKRVCombatUnit()->GetCombatStatus()));
		Blackboard->SetValueAsEnum(NearestEnemyRangeKey, uint8(ECombatStatus::Idle));
		Blackboard->SetValueAsObject(UnitToCheckKey, nullptr);
		Blackboard->SetValueAsVector(PointToCheckKey, FVector::ZeroVector);
	}

}

void AKRVAIController::OnSkillAcuqired(AKRVSkill* Skill, AKRVCombatUnit* ControlledUnit)
{
	if (GetKRVCombatUnit())
	{
		Blackboard->SetValueAsBool(HasActiveSkillKey, GetKRVCombatUnit()->HasActiveSkill());
	}

}

void AKRVAIController::OnActionStatusChanged(EActionStatus NewStatus, AKRVCombatUnit* ControlledUnit)
{
	if (GetKRVCombatUnit())
	{
		Blackboard->SetValueAsEnum(ActionStatusKey, uint8(NewStatus));
	}

}

void AKRVAIController::OnCombatStatusChanged(ECombatStatus NewStatus, AKRVCombatUnit* ControlledUnit)
{
	if (GetKRVCombatUnit())
	{
		Blackboard->SetValueAsEnum(CombatStatusKey, uint8(NewStatus));
	}

}

void AKRVAIController::OnNearestEnemyRangeKeyChanged(ECombatStatus NewStatus, AKRVCombatUnit* ControlledUnit)
{
	if (GetKRVCombatUnit())
	{
		Blackboard->SetValueAsEnum(NearestEnemyRangeKey, uint8(NewStatus));
	}

}

void AKRVAIController::OnTargetUnitChanged(AKRVCombatUnit* TargetUnit, AKRVCombatUnit* ControlledUnit)
{
	if (GetKRVCombatUnit())
	{
		Blackboard->SetValueAsObject(TargetUnitKey, TargetUnit);
		if (TargetUnit)
		{
			Blackboard->SetValueAsInt(TargetUnitIDKey, TargetUnit->GetUnitInFieldID());
		}
		else
		{
			Blackboard->SetValueAsInt(TargetUnitIDKey, -1);
		}
	}

}

void AKRVAIController::OnTargetPointChanged(const FVector& NewPoint, AKRVCombatUnit* ControlledUnit)
{
	if (GetKRVCombatUnit())
	{
		Blackboard->SetValueAsVector(TargetPointKey, NewPoint);
	}

}
