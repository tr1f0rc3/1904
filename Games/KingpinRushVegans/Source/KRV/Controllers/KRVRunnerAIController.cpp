// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVRunnerAIController.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "KRV.h"
#include "DataAssets/KRVCharacterDataAsset.h"
#include "ActorComponents/KRVCharacterStatComponent.h"
#include "Pawns/KRVCharacter.h"
#include "Actors/KRVPath.h"

const FName AKRVRunnerAIController::PathToFollowKey(TEXT("PathToFollow"));
const FName AKRVRunnerAIController::SplineTimeKey(TEXT("SplineTime"));
const FName AKRVRunnerAIController::IsFlyingKey(TEXT("IsFlying"));
const FName AKRVRunnerAIController::WalkingDistKey(TEXT("WalkingDist"));

AKRVRunnerAIController::AKRVRunnerAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Resources/AI/BB_EnemyCharacter.BB_EnemyCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Resources/AI/BT_Runner.BT_Runner"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void AKRVRunnerAIController::OnPossess(APawn* InPawn)
{
	AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(InPawn);
	KRVCHECK(KRVCharacter);
	if (UseBlackboard(BBAsset, Blackboard) && KRVCharacter)
	{
		KRVCHECK(KRVCharacter->PathToFollow.IsValid());
		if (KRVCharacter->PathToFollow.IsValid())
		{
			Blackboard->SetValueAsObject(PathToFollowKey, KRVCharacter->GetPathToFollow());
		}
		if (KRVCharacter->GetCharacterDataAsset())
		{
			Blackboard->SetValueAsBool(IsFlyingKey, KRVCharacter->GetCharacterDataAsset()->bIsFlying);
		}
		Blackboard->SetValueAsFloat(WalkingDistKey, 0.f);
		Blackboard->SetValueAsFloat(SplineTimeKey, KRVCharacter->SplineTime);
	}

	Super::OnPossess(InPawn);

}