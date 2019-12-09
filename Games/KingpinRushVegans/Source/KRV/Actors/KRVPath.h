// Fill out your copyright notice in the Description page of Project Settings.
	// reference (kr): http://alleysark.kr/how-to-make-spline-following-ai-with-behavior-tree/

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRVEnums.h"

#include "KRVPath.generated.h"

UCLASS()
class KRV_API AKRVPath : public AActor
{
	GENERATED_BODY()
	
public:	
	AKRVPath();

protected:
	virtual void PostInitializeComponents() override;
	virtual void BeginPlay() override;

public:
	UFUNCTION(BlueprintCallable)
		int32 GetPathID() const;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UBoxComponent* BoxComponent;

	// this initial value should be assigned and unique
	// 초기 값을 꼭 배정해야 하며, 고유해야 함.
	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "ID")
		int32 PathID;

public: // Path
	UFUNCTION(BlueprintCallable)
		bool IsPathClosedLoop() const;

	UFUNCTION(BlueprintCallable)
		float GetPathDuration() const;

	UFUNCTION(BlueprintCallable)
		bool GetPathLocationAtTime(FVector& OutLocation, float CurrentTime);

	UFUNCTION(BlueprintCallable)
		float FindClosestTimeToWorldLocation(const FVector& NewLocation);

	UFUNCTION(BlueprintCallable)
		float ComputeTimeSepartedFrom(float CurrentTime, const FVector& CurrentLocation,
			float DistThreshold = 100.f, float UpdatePrecision = 0.01f);

	void ShowIndicators(bool bIsTrue);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		TSubclassOf<class AKRVSplineIndicator> SplineIndicatorClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		TArray<TSoftObjectPtr<class AKRVSplineIndicator>> SplineIndicators;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
		class USplineComponent* SplineComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Path")
		float IndicatorDistInterval = 100.f;

};
