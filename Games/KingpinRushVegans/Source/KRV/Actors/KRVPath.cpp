// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVPath.h"
#include "Components/SplineComponent.h"
#include "Components/BoxComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"

#include "Pawns/KRVCharacter.h"
#include "KRVSplineIndicator.h"


// Sets default values
AKRVPath::AKRVPath()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX_COMPONENT"));
	BoxComponent->InitBoxExtent(FVector(10.f, 10.f, 50.f));
	SetRootComponent(BoxComponent);
	SetActorEnableCollision(false);

	SplineComponent = CreateDefaultSubobject<USplineComponent>(TEXT("SPLINE_COMPONENT"));
	SplineComponent->SetupAttachment(RootComponent);
	SplineComponent->AddRelativeLocation(FVector(0.f, 0.f, 50.f));

}

void AKRVPath::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (SplineComponent)
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		int32 NumOfIndicators = FMath::CeilToInt(SplineComponent->GetSplineLength() / IndicatorDistInterval);
		float TimeInterval = 1.f / float(NumOfIndicators);
		float TimeToSpawn = -TimeInterval;
		FVector OutVector;
		for (int32 Index = 0; Index < NumOfIndicators; ++Index)
		{
			TimeToSpawn += TimeInterval;
			AKRVSplineIndicator* NewSplineIndicator = GetWorld()->SpawnActor<AKRVSplineIndicator>
				(AKRVSplineIndicator::StaticClass(), OutVector, FRotator::ZeroRotator, SpawnParameters);
			NewSplineIndicator->SetPathToFollow(this);
			NewSplineIndicator->SetCurrentTime(TimeToSpawn);
			SplineIndicators.Emplace(NewSplineIndicator);
		}
	}

}

void AKRVPath::BeginPlay()
{
	Super::BeginPlay();

}

int32 AKRVPath::GetPathID() const
{
	return PathID;

}

bool AKRVPath::IsPathClosedLoop() const
{
	KRVCHECK(SplineComponent);
	return SplineComponent->IsClosedLoop();


}

float AKRVPath::GetPathDuration() const
{
	KRVCHECK(SplineComponent);
	return SplineComponent->Duration;

}

bool AKRVPath::GetPathLocationAtTime(FVector& OutLocation, float CurrentTime)
{
	if (!SplineComponent || CurrentTime < 0.f)
	{
		KRVCHECK(SplineComponent);
		KRVCHECK(CurrentTime < 0.f);
		return false;
	}
	else
	{
		OutLocation = SplineComponent->GetLocationAtTime(CurrentTime, ESplineCoordinateSpace::World, true);
		return true;
	}
}

float AKRVPath::FindClosestTimeToWorldLocation(const FVector& NewLocation)
{
	KRVCHECK(SplineComponent);
	float TimeInputKey = SplineComponent->FindInputKeyClosestToWorldLocation(NewLocation);
	int32 NumberOfSplinePoint = SplineComponent->GetNumberOfSplinePoints();
	NumberOfSplinePoint = (IsPathClosedLoop()) ? NumberOfSplinePoint : --NumberOfSplinePoint;
	float SplinePointsPerSec = float(NumberOfSplinePoint) / SplineComponent->Duration;
	return TimeInputKey / SplinePointsPerSec;

}

float AKRVPath::ComputeTimeSepartedFrom(float CurrentTime, const FVector& CurrentLocation, float DistThreshold /*= 100.f*/, float UpdatePrecision /*= 0.01f*/)
{
	if (!SplineComponent || CurrentTime < 0.f)
	{
		KRVCHECK(SplineComponent);
		KRVCHECK(CurrentTime < 0.f);
	}
	else
	{
		FVector PathLocationAtTime =
			SplineComponent->GetLocationAtTime(CurrentTime, ESplineCoordinateSpace::World, true);
		if (FVector::DistSquared2D(CurrentLocation, PathLocationAtTime) >=
			FMath::Square(DistThreshold))
		{
			return CurrentTime;
		}
		else
		{
			float UpdatedTime = CurrentTime;
			bool bShouldUpdateTime = false;
			bool bDoesUpdateInLoopBegin = false;
			FVector UpdatedLocation = FVector::ZeroVector;

			// Whether check the time reached end of time.
			// �ð��� ���� �ٴٶ����� Ȯ��.
			if (IsPathClosedLoop())
			{
				// If it is closed loop, update time at least once.
				// ��ȯ�� ��� �� ���� ���ö��� �ð��� ������Ʈ��
				bShouldUpdateTime = !FMath::IsNearlyEqual(UpdatedTime, CurrentTime, UpdatePrecision) ||
					!bDoesUpdateInLoopBegin;
			}
			else
			{
				bShouldUpdateTime = UpdatedTime < SplineComponent->Duration;
			}

			while (bShouldUpdateTime)
			{
				// Advance the time. Rewind or clamp the time if it reaches end of time.
				// �ð� �� ���� �մ��. ���� �ٴٸ��� �ǰ��ų� Ŭ�����Ѵ�.
				UpdatedTime += UpdatePrecision;
				if (UpdatedTime > SplineComponent->Duration)
				{
					if (IsPathClosedLoop())
					{
						UpdatedTime -= SplineComponent->Duration;
						bDoesUpdateInLoopBegin = true;
					}
					else
					{
						return SplineComponent->Duration;
					}
				}

				// Check Whether the location at the updated time sufficiently separated.
				// ������Ʈ �� �ð��� ������ ���� ������ ����� �������ִ��� Ȯ����.
				UpdatedLocation = SplineComponent->GetLocationAtTime(UpdatedTime, ESplineCoordinateSpace::World, true);
				if (FVector::DistSquared2D(CurrentLocation, UpdatedLocation) >=
					FMath::Square(DistThreshold))
				{
					return UpdatedTime;
				}
			}
			return SplineComponent->Duration;
		}
	}
	return -1.f;

}

void AKRVPath::ShowIndicators(bool bIsTrue)
{
	if (bIsTrue)
	{
		for (TSoftObjectPtr<AKRVSplineIndicator> SplineIndicator : SplineIndicators)
		{
			if (SplineIndicator.IsValid())
			{
				SplineIndicator.Get()->ShowThisAndFollowPath();
			}
		}
	}
	else
	{
		for (TSoftObjectPtr<AKRVSplineIndicator> SplineIndicator : SplineIndicators)
		{
			if (SplineIndicator.IsValid())
			{
				SplineIndicator->HideThis();
			}
		}
	}

}

