// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVSplineIndicator.h"
#include "Components/StaticMeshComponent.h"

#include "KRV.h"
#include "KRVPath.h"
#include "Kismet/KismetMathLibrary.h"

AKRVSplineIndicator::AKRVSplineIndicator()
{
	PrimaryActorTick.bCanEverTick = true;

	BodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY_MESH_COMPONENT"));
	SetRootComponent(BodyMeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> BODY_MESH_OBJ
	(TEXT("/Game/Resources/StaticMeshes/SM_SplineIndicator.SM_SplineIndicator"));
	if (BODY_MESH_OBJ.Succeeded())
	{
		BodyMeshComponent->SetStaticMesh(BODY_MESH_OBJ.Object);
		BodyMeshComponent->SetRelativeScale3D(BaseBodyMeshScale * FVector::OneVector);
	}

	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	SplineTimeIntervalPerSec = 1 / SecondsToCompleteRun;

}

void AKRVSplineIndicator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bIsShown)
	{
		AKRVPath* PathToFollow = GetPathToFollow();
		if (PathToFollow)
		{
			// move the actor forward
			CurrentSplineTime += SplineTimeIntervalPerSec * DeltaTime;
			if (CurrentSplineTime > 1.f)
			{
				CurrentSplineTime = 0.f;
			}
			SetCurrentTime(CurrentSplineTime);

			// set body mesh scale
			CurrentTimeVaratingBodyMesh += 3 * DeltaTime;
			if (CurrentTimeVaratingBodyMesh > 7.f) // more than 2 * pi
			{
				CurrentTimeVaratingBodyMesh -= 7.f;
			}
			BodyMeshComponent->SetRelativeScale3D
			((BaseBodyMeshScale + VarationBodyMeshScale * FMath::Cos(CurrentTimeVaratingBodyMesh))
			* FVector::OneVector);
		}
	}

}

void AKRVSplineIndicator::SetPathToFollow(AKRVPath* NewPath)
{
	KRVCHECK(NewPath);
	SoftPtr_PathToFollow = NewPath;

}

bool AKRVSplineIndicator::ShowThisAndFollowPath()
{
	AKRVPath* PathToFollow = GetPathToFollow();
	if (!PathToFollow)
	{
		KRVCHECK(PathToFollow);
	}
	else
	{
		SetActorTickEnabled(true);
		SetActorHiddenInGame(false);
		bIsShown = true;
		return true;
	}
	return false;

}

void AKRVSplineIndicator::HideThis()
{
	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	bIsShown = false;

}

bool AKRVSplineIndicator::SetCurrentTime(float TimeToSet)
{
	AKRVPath* PathToFollow = GetPathToFollow();
	if (!PathToFollow)
	{
		KRVCHECK(PathToFollow);
	}
	else
	{
		TimeToSet = FMath::Clamp(TimeToSet, 0.f, 1.f);
		if (PathToFollow->GetPathLocationAtTime(OutVector, TimeToSet))
		{
			CurrentSplineTime = TimeToSet;
			SetActorLocation(OutVector);
			return true;
		}
	}
	return false;

}

class AKRVPath* AKRVSplineIndicator::GetPathToFollow() const
{
	if (SoftPtr_PathToFollow.IsValid())
	{
		return SoftPtr_PathToFollow.Get();
	}
	else
	{
		KRVCHECK(SoftPtr_PathToFollow.IsValid());
		return nullptr;
	}

}

