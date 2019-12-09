// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVCharacterAIController.h"

#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "KRV.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"

AKRVCharacterAIController::AKRVCharacterAIController()
{
	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Resources/AI/BT_Character.BT_Character"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void AKRVCharacterAIController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

}

AKRVCharacter* AKRVCharacterAIController::GetCharacter() const
{
	return Cast<AKRVCharacter>(GetPawn());

}


