// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Structs/KRVCharacterSpawnData.h"
#include "SpawnerComponent.generated.h"

class UPooledComponent;
class USoundCue;
class UBoxComponent;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSpawnCharacterDelegate, AKRVCharacter*);

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class KRV_API USpawnerComponent : public USceneComponent
{
	GENERATED_BODY()

public:
	USpawnerComponent();

protected:
	virtual void InitializeComponent() override;

public:
	UFUNCTION(BlueprintCallable)
		AActor* Spawn();

	UFUNCTION(BlueprintCallable)
		void StopSpawning();

	UFUNCTION(BlueprintCallable)
		void SpawnWithOutArray(TArray<AActor*>& OutActors);

	// 0 times means infinitely spawn.
	// 0 회 지정하면 무한히 호출.
	UFUNCTION(BlueprintCallable)
		void SetSpawnParams(const FVector& _Location, const FRotator& _Rotation, bool _IsRelative, int32 _Times = 1, float _InRate = 0.1f);

	UFUNCTION(BlueprintCallable)
		void SetSpawnLocAndRot(const FVector& _Location, const FRotator& _Rotation, bool _IsRelative);

	UFUNCTION(BlueprintCallable)
		void SetSpawnRate(int32 _Times, float _InRate);

	UFUNCTION(BlueprintCallable)
		bool IsPoolSet() const;

	UFUNCTION(BlueprintCallable)
		TSubclassOf<AActor> GetPooledActorClass() const;

	UFUNCTION(BlueprintCallable)
		int32 GetPoolSize() const;

	UFUNCTION(BlueprintCallable)
		bool GetPool(TArray<AActor*>& OutPool) const;

	UFUNCTION(BlueprintCallable)
		AActor* GetFirstSample() const;

	UFUNCTION(BlueprintCallable)
		bool GetActiveActors(TArray<AActor*>& OutActors) const;

	UFUNCTION(BlueprintCallable)
		void SetPool(TSubclassOf<AActor> _Class, int32 _PoolSize);

	UFUNCTION(BlueprintCallable)
		void SetPoolWithCharacterSpawnData(const FKRVCharacterSpawnData& Data);

	// cost O(m*n);
	UFUNCTION(BlueprintCallable)
		void ExtendPool(int32 ExtendSize = 1);

	UFUNCTION(BlueprintCallable)
		void DestroyPool();

	template <class T = AActor>
	FORCEINLINE bool GetPool_Template(TArray<T*>& OutPool) const
	{
		if (ActorPool.Num() <= 0)
		{
			KRVCHECK(ActorPool.Num() > 0);
			return false;
		}
		else
		{
			OutPool.Empty();
			for (AActor* ActorInPool : ActorPool)
			{
				OutPool.Emplace(Cast<T>(ActorInPool));
			}
			return true;
		}

	}


public: // PooledComponent call these fcns.
	// 풀드 컴포넌트가 호출하는 함수들
	UPooledComponent* GetPooledComponent();
	void CheckOutDeactivatedObject(int32 IndexToBeDeactivated);

protected:
	UPooledComponent* InitPooledActor(int32 PoolIndex);
	FVector GetRandomPointInBox() const;
	UPooledComponent* GetPooledComponentFromActor(AActor* PooledActor) const;
	AActor* SpawnOnce();
	void SpawnRecursive();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		float SpawnedActorLifeSpan = 0.f;

public:
	FOnSpawnCharacterDelegate OnSpawnCharacterDelegate;

protected: // Pool
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
		TSubclassOf<AActor> PooledActorClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pool")
		int32 PoolSize;

	UPROPERTY()
		TArray<AActor*> ActorPool;

	UPROPERTY()
		TArray<UPooledComponent*> ComponentPool;

	bool bIsPoolSet;
	bool bIsInLoop;
	int32 FirstAvailableIndex;
	int32 LastAvailableIndex;

	int32 RepeatingCallsRemaining;
	FTimerHandle SpawnCooldownTimer;

public: // Spawn time and space
	UFUNCTION(BlueprintCallable, Category = "Transform")
		void SetBoxSize(const FVector& NewSize);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
		FVector SpawnLocation = FVector(150.f, 0.f, 0.f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
		FRotator SpawnRotation = FRotator::ZeroRotator;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Transform")
		bool bIsRelative = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		float InRate = 0.3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		int32 RepeatingTime = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Time")
		FVector BoxSize = FVector::ZeroVector;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Character")
		int32 SpareCharacters = 2;

};
