// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_SelectSkillAndSetTarget.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Actors/KRVSkill.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVAIController.h"

UBTTask_SelectSkillAndSetTarget::UBTTask_SelectSkillAndSetTarget()
{
	bNotifyTick = true;
	NodeName = TEXT("SelectSkillAndSetTarget");

}

EBTNodeResult::Type UBTTask_SelectSkillAndSetTarget::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	EBTNodeResult::Type Result = Super::ExecuteTask(OwnerComp, NodeMemory);

	AKRVCombatUnit* KRVCaster = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());

	if (!KRVCaster)
	{
		KRVCHECK(KRVCaster);
		return EBTNodeResult::Failed;
	}
	else
	{
		
		for (int32 Index = 0;; ++Index)
		{
			FGameplayTag SkillTagToCast;
			if (!KRVCaster->GetSkillTagByIndex(SkillTagToCast, Index))
			{
				break;
			}
			else
			{
				// Set Values
				AKRVSkill* SkillToCast = KRVCaster->GetSkillByTag(SkillTagToCast);
				ETargetType SkillTargetType = SkillToCast->GetTargetType();
				float SkillRadius = SkillToCast->GetSkillRadius();
				float SkillRange = SkillToCast->GetSkillRange();
				ETargetingType TargetingType = SkillToCast->GetTargetingType();

				if (!KRVCaster->IsSkillReady(SkillTagToCast) || !SkillToCast->IsAutoCastable())
				{
					continue;
				}
				else
				{
					TArray<AKRVCombatUnit*> TargetsToCheck;

					if (TargetingType == ETargetingType::Self)
					{
						if (SkillTargetType == ETargetType::ToSelf)
						{
							KRVCaster->SetReservedSkill(SkillTagToCast);
							return EBTNodeResult::Succeeded;
						}
						else if (UKRVBlueprintFunctionLibrary::MultiRadialTrace_Template<AKRVCombatUnit>
							(KRVCaster, TargetsToCheck, KRVCaster->GetActorLocation(), SkillRadius, SkillTargetType))
						{
							KRVCaster->SetReservedSkill(SkillTagToCast);
							return EBTNodeResult::Succeeded;
						}
					}
					else if (KRVCaster->IsInSkillCastRange(SkillTagToCast))
					{
						KRVCaster->SetReservedSkill(SkillTagToCast);

						switch (TargetingType)
						{
						case ETargetingType::Target:
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsObject(AKRVAIController::UnitToCheckKey, KRVCaster->GetTargetUnit());
							return EBTNodeResult::InProgress;
						}
						case ETargetingType::Location:
						{
							OwnerComp.GetBlackboardComponent()->SetValueAsVector(AKRVAIController::PointToCheckKey, KRVCaster->GetTargetPoint());
							return EBTNodeResult::InProgress;
						}
						default:
							break;
						}
						return EBTNodeResult::InProgress;
					}
					else // find new target
					{
						AKRVCombatUnit* TargetUnit = nullptr;
						TArray<AKRVCharacter*> CharactersToCheck;

						// Get characters to turn
						if (SkillTargetType == ETargetType::FriendlyCharacter)
						{
							KRVCaster->FindVisibleCharactersWithTurn(CharactersToCheck, SkillRange, true);
						}
						else if (SkillTargetType == ETargetType::HostileCharacter)
						{
							KRVCaster->FindVisibleCharactersWithTurn(CharactersToCheck, SkillRange, false);
						}
						else
						{
							UKRVBlueprintFunctionLibrary::MultiRadialTrace_Template<AKRVCombatUnit>
								(KRVCaster, TargetsToCheck, KRVCaster->GetActorLocation(), SkillRange, SkillTargetType);
						}

						// find characters nearest from forward vector.
						// 가장 시야에서 적게 벗어난 대상을 찾음.
						TargetUnit = Cast<AKRVCombatUnit>(KRVCaster->FindTheNearestFromFowardVectorInSkillCastRange(SkillTagToCast, CharactersToCheck));
						if (TargetUnit)
						{
							KRVCaster->SetReservedSkill(SkillTagToCast);

							switch (TargetingType)
							{
							case ETargetingType::Target:
							{
								OwnerComp.GetBlackboardComponent()->SetValueAsObject(AKRVAIController::UnitToCheckKey, TargetUnit);
								return EBTNodeResult::InProgress;
							}
							case ETargetingType::Location:
							{
								OwnerComp.GetBlackboardComponent()->SetValueAsVector(AKRVAIController::PointToCheckKey, TargetUnit->GetActorLocation());
								return EBTNodeResult::InProgress;
							}
							default:
								break;
							}
						}
					}
				}
			}
		}
	}
	return EBTNodeResult::Failed;

}

void UBTTask_SelectSkillAndSetTarget::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickTask(OwnerComp, NodeMemory, DeltaSeconds);

	AKRVCombatUnit* KRVCaster = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());
	if (!KRVCaster)
	{
		KRVCHECK(KRVCaster);
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (KRVCaster->GetActionStatus() == EActionStatus::Dead)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
	}
	else if (KRVCaster->GetActionStatus() == EActionStatus::Stunned)
	{
		// InProgress
	}
	else
	{
		AKRVSkill* SkillToCast = KRVCaster->GetSkillByTag(KRVCaster->GetReservedSkillTag());
		ETargetingType TargetingType = SkillToCast->GetTargetingType();
		FVector ForwardVectorTo;

		if (!SkillToCast)
		{
			KRVCHECK(SkillToCast);
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		}
		else
		{
			switch (TargetingType)
			{
			case ETargetingType::Target:
			{
				AKRVCombatUnit* UnitToFace = Cast<AKRVCombatUnit>
					(OwnerComp.GetBlackboardComponent()->GetValueAsObject(AKRVAIController::UnitToCheckKey));
				if (!UnitToFace)
				{
					KRVCHECK(UnitToFace);
					FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
					return;
				}
				else
				{
					ForwardVectorTo = (UnitToFace->GetActorLocation() -
						KRVCaster->GetActorLocation()).GetSafeNormal2D();
				}
				break;
			}
			case ETargetingType::Location:
			{
				FVector LocationToFace = 
					(OwnerComp.GetBlackboardComponent()->GetValueAsVector(AKRVAIController::PointToCheckKey));

				if (LocationToFace == FVector::ZeroVector)
				{
					KRVCHECK(LocationToFace != FVector::ZeroVector);
					FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
					return;
				}
				else
				{
					ForwardVectorTo = (LocationToFace -	KRVCaster->GetActorLocation()).GetSafeNormal2D();
				}
				break;
			}
			default:
				break;
			}

			FVector CurrentForwardVector = KRVCaster->GetActorForwardVector().GetSafeNormal2D();
			if (FVector::DotProduct(CurrentForwardVector, ForwardVectorTo) > 0.95f)
			{
				FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
			}
			else // InProgress
			{
				FRotator RotationToTurn = FRotationMatrix::MakeFromX(ForwardVectorTo).Rotator();
				KRVCaster->SetActorRotation(FMath::RInterpTo
				(KRVCaster->GetActorRotation(), RotationToTurn, DeltaSeconds, 3.0f));
			}
		}
	}

}

