// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRVSplineIndicator.generated.h"

UCLASS()
class KRV_API AKRVSplineIndicator : public AActor
{
	GENERATED_BODY()

public:
	AKRVSplineIndicator();

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UStaticMeshComponent* BodyMeshComponent;

public:
	void SetPathToFollow(class AKRVPath* NewPath);
	bool ShowThisAndFollowPath();
	void HideThis();

protected:
	class AKRVPath* GetPathToFollow() const;

protected: // Bodymesh
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float BaseBodyMeshScale = 0.4f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float VarationBodyMeshScale = 0.1f;

	float CurrentTimeVaratingBodyMesh = 0.f;

public: // spline
	bool SetCurrentTime(float TimeToSet);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		TWeakObjectPtr<AKRVPath> SoftPtr_PathToFollow;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float SecondsToCompleteRun = 20.f;

	float SplineTimeIntervalPerSec;
	bool bIsShown;

	float CurrentSplineTime;
	FVector OutVector;

};
