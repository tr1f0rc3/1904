// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVEnemyAIController.h"
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

AKRVEnemyAIController::AKRVEnemyAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Resources/AI/BB_EnemyCharacter.BB_EnemyCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Resources/AI/BT_EnemyCharacter.BT_EnemyCharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void AKRVEnemyAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

}