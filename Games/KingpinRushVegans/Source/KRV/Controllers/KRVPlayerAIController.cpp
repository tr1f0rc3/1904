// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVPlayerAIController.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"

#include "KRV.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVInGamePlayerController.h"

const FName AKRVPlayerAIController::IsRecruitedByTowerKey(TEXT("IsRecruitedByTower"));

AKRVPlayerAIController::AKRVPlayerAIController()
{
	static ConstructorHelpers::FObjectFinder<UBlackboardData> BBObject(TEXT("/Game/Resources/AI/BB_PlayerCharacter.BB_PlayerCharacter"));
	if (BBObject.Succeeded())
	{
		BBAsset = BBObject.Object;
	}

	static ConstructorHelpers::FObjectFinder<UBehaviorTree> BTObject(TEXT("/Game/Resources/AI/BT_PlayerCharacter.BT_PlayerCharacter"));
	if (BTObject.Succeeded())
	{
		BTAsset = BTObject.Object;
	}

}

void AKRVPlayerAIController::OnPossess(APawn* InPawn)
{
	AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(InPawn);
	KRVCHECK(KRVCharacter);
	if (UseBlackboard(BBAsset, Blackboard) && KRVCharacter)
	{
		Blackboard->SetValueAsBool(IsRecruitedByTowerKey, KRVCharacter->bIsRecruitedByTower);
	}

	Super::OnPossess(InPawn);

}

