// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_FindAndSetNearestEnemy.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVAIController.h"

UBTService_FindAndSetNearestEnemy::UBTService_FindAndSetNearestEnemy()
{
	NodeName = TEXT("FindAndSetNearestEnemy");
	Interval = 0.4f;
}

void UBTService_FindAndSetNearestEnemy::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	AKRVCombatUnit* UnitOnGuard = Cast<AKRVCombatUnit>(OwnerComp.GetAIOwner()->GetPawn());

	if (UnitOnGuard)
	{
		ECombatStatus CombatStatus = UnitOnGuard->GetCombatStatus();
	}

}
