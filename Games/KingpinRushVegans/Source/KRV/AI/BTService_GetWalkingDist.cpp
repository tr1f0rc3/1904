// Fill out your copyright notice in the Description page of Project Settings.


#include "BTService_GetWalkingDist.h"
#include "Components/SplineComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

#include "KRV.h"
#include "Pawns/KRVCharacter.h"
#include "Controllers/KRVRunnerAIController.h"

UBTService_GetWalkingDist::UBTService_GetWalkingDist()
{
	NodeName = TEXT("GetWalkingDist");
	Interval = 0.1f;

	RecendDists.Init(0, TicksToCheck);
	CurrentIndex = 0;
	RecentLocation = FVector::ZeroVector;
	SumOfRecentDists = 0.f;

}

void UBTService_GetWalkingDist::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	if (!KRVRunner)
	{
		KRVRunner = Cast<AKRVCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	}
	if (KRVRunner)
	{
		FVector CurrentLocation = KRVRunner->GetActorLocation();
		SumOfRecentDists -= RecendDists[CurrentIndex];

		RecendDists[CurrentIndex] = FVector::Dist2D(CurrentLocation, RecentLocation);
		SumOfRecentDists += RecendDists[CurrentIndex];
		RecentLocation = CurrentLocation;

		if (++CurrentIndex >= TicksToCheck)
		{
			CurrentIndex = 0;
		}

		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(AKRVRunnerAIController::WalkingDistKey, SumOfRecentDists);
	}

}
