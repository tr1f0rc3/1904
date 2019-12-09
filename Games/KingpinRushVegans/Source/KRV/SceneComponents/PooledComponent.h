// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PooledComponent.generated.h"

DECLARE_MULTICAST_DELEGATE(FOnNearActorActivateDelegate);
DECLARE_MULTICAST_DELEGATE(FOnNearActorDeactivateDelegate);

class USpawnerComponent;
class UAudioComponent;
class UParticleSystemComponent;
class UParticleSystem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KRV_API UPooledComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	UPooledComponent();

public:
	virtual void InitializeComponent() override;

public: // Spawner component and index
	UFUNCTION(BlueprintCallable, Category = "Spawner")
		USpawnerComponent* GetSpawnerComponent() const;

	void SetSpawnerComponent(USpawnerComponent* _SpawnerComp);

	UFUNCTION(BlueprintCallable, Category = "Pool")
		int32 GetPoolIndex() const;

	void SetPoolIndex(int32 Index);

protected:
	UPROPERTY(Transient, BlueprintReadOnly)
		bool bHasSpawnerComponent;

	UPROPERTY(Transient, BlueprintReadOnly)
		TWeakObjectPtr<USpawnerComponent> SpawnerComponent;

	// PoolIndex would be assigned by SpawnerComp.
	// Not assigned value means it has no or dummy SpawnerComp 
	int32 PoolIndex = -1;

public: // owner active / deactive
	UFUNCTION(BlueprintCallable, Category = "Pool")
		void DeactivateNearActor();

	// If LifeSpan is 0.f (default), the owner is persistent.
	// 0.f초면 영구지속.
	UFUNCTION(BlueprintCallable, Category = "Pool")
		void SetNearActorLifeSpan(float NewLifeSpan);

	UFUNCTION(BlueprintCallable, Category = "Pool")
		bool IsNearActorActive() const;

	// just hide/show actor
	UFUNCTION(BlueprintCallable)
		void HideNearActor(bool bIsTrue);

	void SetNearActorActive(bool IsActive);

protected:
	void OnDestroyTimeUp();

public:
	FOnNearActorActivateDelegate OnNearActorActivateDelegate;
	FOnNearActorDeactivateDelegate OnNearActorDeactivateDelegate;

public: // Effects sharing within class.
	// 특정 클래스가 공유하는 효과들 
	UPROPERTY(BlueprintReadOnly, Category = "Spawn")
		UAudioComponent* OnSpawnedAudioComponent;

	UPROPERTY(BlueprintReadOnly, Category = "Spawn")
		UParticleSystemComponent* OnSpawnedParticleComponent;

	UPROPERTY(BlueprintReadWrite, Category = "Spawn")
		UParticleSystem* OnDestroyedParticle;

	// Immediate destroy cause crash with particle deactivation, referencing null ptr.
	// Set time to delay destroy.
	// 즉시 디스트로이하면 파티클 비활성화가 널포인터를 참조해 크래쉬가 남.
	UPROPERTY(BlueprintReadWrite, Category = "Spawn")
		float TimeToDestroy = 10.f;

protected:
	// If LifeSpan is 0.f (default), the owner is persistent.
	// 0.f초면 영구지속.
	float OwnerLifeSpan;
	FTimerHandle LifeSpanTimer;
	FTimerHandle DestroyDelayTimer;
	bool bIsOwnerActive;

public: // NearActor
	UFUNCTION(BlueprintCallable, Category = "Outer")
		AActor* GetNearActor();

protected:
	void SetNearActor();

protected:
	UPROPERTY()
		AActor* FoundNearActor;

};
