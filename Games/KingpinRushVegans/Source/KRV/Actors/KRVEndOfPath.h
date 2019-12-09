// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "KRVEnums.h"
#include "KRVEndOfPath.generated.h"

DECLARE_DELEGATE_OneParam(FOnEnemyEscapeDelegate, class AKRVCharacter*);

UCLASS()
class KRV_API AKRVEndOfPath : public AActor
{
	GENERATED_BODY()

public:
	AKRVEndOfPath();

public:
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UBoxComponent* BoxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FVector BoxComponentSize = FVector(200.f, 200.f, 1000.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		class UStaticMeshComponent* BodyMeshComponent;

	FName BodyMeshCenterSocketName = "CenterSocket";

protected: //Overlap
	UFUNCTION()
		void OnKRVCharacterBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	FOnEnemyEscapeDelegate OnEnemyEscapeDelegate;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UParticleSystem* OnBeginOverlapParticle;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UAudioComponent* OnHeartTakenAudioComponent;

protected: // Endgame
	UFUNCTION()
		void OnGameEnd(EKRVTeam Winner);

	UFUNCTION()
		void ActivateAfterBrokenParticleComponent();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UParticleSystemComponent* OnBrokenParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UParticleSystemComponent* AfterBrokenParticleComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		float AfterBrokenParticleActivationDelay = 1.f;

	FTimerHandle OnBrokenParticleDelayTimer;

};
