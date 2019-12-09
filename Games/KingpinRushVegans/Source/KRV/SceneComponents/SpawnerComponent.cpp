// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnerComponent.h"
#include "TimerManager.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"
#include "Sound/SoundCue.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "PooledComponent.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "GameModes/KRVInGameGameMode.h"

USpawnerComponent::USpawnerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	bIsPoolSet = false;
	bIsInLoop = false;

}

void USpawnerComponent::InitializeComponent()
{
	Super::InitializeComponent();
	SetPool(PooledActorClass, PoolSize);

}

AActor* USpawnerComponent::Spawn()
{
	if (!bIsPoolSet)
	{
		KRVLOG(Error, TEXT("Pool is not set"));
	}
	else if (RepeatingTime <= 0)
	{
		KRVLOG(Error, TEXT("RepeatingTime should be positive"));
	}
	else if ((RepeatingTime != 1 || bIsInLoop) && InRate <= 0)
	{
		KRVLOG(Error, TEXT("Invalid InRate"));
	}
	else
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(SpawnCooldownTimer);

		if (RepeatingTime == 1)
		{
			return SpawnOnce();
		}
		else
		{
			RepeatingCallsRemaining = RepeatingTime;
			SpawnRecursive();
		}
	}
	return nullptr;

}

void USpawnerComponent::StopSpawning()
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(SpawnCooldownTimer);
	}

}

void USpawnerComponent::SpawnWithOutArray(TArray<AActor*>& OutActors)
{
	OutActors.Empty();
	int32 AvailableIndex = -1;
	if (FirstAvailableIndex <= LastAvailableIndex)
	{
		for (int32 Index = FirstAvailableIndex;
			 Index < FirstAvailableIndex + RepeatingTime && Index <= LastAvailableIndex; ++Index)
		{
			OutActors.Emplace(ActorPool[Index]);
		}
	}
	else
	{
		for (int32 Index = FirstAvailableIndex; 
			Index < FirstAvailableIndex + RepeatingTime && Index <= ActorPool.Num() - 1; ++Index)
		{
			OutActors.Emplace(ActorPool[Index]);
		}
		if (FirstAvailableIndex + RepeatingTime > ActorPool.Num())
		{
			for (int32 Index = 0;
				Index < RepeatingTime - (ActorPool.Num() - FirstAvailableIndex)
				&& Index <= LastAvailableIndex; ++Index)
			{
				OutActors.Emplace(ActorPool[Index]);
			}
		}
		
	}

	Spawn();

}

void USpawnerComponent::SetSpawnParams(const FVector& _Location, const FRotator& _Rotation, bool _IsRelative, int32 _Times, float _InRate)
{
	SetSpawnLocAndRot(_Location, _Rotation, _IsRelative);
	SetSpawnRate(_Times, _InRate);

}

void USpawnerComponent::SetSpawnLocAndRot(const FVector& _Location, const FRotator& _Rotation, bool _IsRelative)
{
	SpawnLocation = _Location;
	SpawnRotation = _Rotation;
	bIsRelative = _IsRelative;

}

void USpawnerComponent::SetSpawnRate(int32 _Times, float _InRate)
{
	bIsInLoop = (_Times == 0) ? true : false;
	InRate = _InRate;
	RepeatingTime = _Times;

}

bool USpawnerComponent::IsPoolSet() const
{
	return bIsPoolSet;

}

TSubclassOf<AActor> USpawnerComponent::GetPooledActorClass() const
{
	return PooledActorClass;

}

int32 USpawnerComponent::GetPoolSize() const
{
	return PoolSize;

}

bool USpawnerComponent::GetPool(TArray<AActor*>& OutPool) const
{
	if (ActorPool.Num() <= 0)
	{
		KRVCHECK(ActorPool.Num() > 0);
	}
	else
	{
		OutPool.Empty();
		for (auto ActorInPool : ActorPool)
		{
			OutPool.Emplace(ActorInPool);
		}
		return true;
	}
	return false;

}

AActor* USpawnerComponent::GetFirstSample() const
{
	if (ActorPool.Num() <= 0)
	{
		KRVCHECK(ActorPool.Num() > 0);
	}
	else
	{
		return ActorPool[0];
	}
	return nullptr;

}

bool USpawnerComponent::GetActiveActors(TArray<AActor*>& OutActors) const
{
	if (FirstAvailableIndex == LastAvailableIndex)
	{
		GetPool(OutActors);
		return true;
	}
	else
	{
		OutActors.Empty();
		if (LastAvailableIndex < PoolSize)
		{
			for (int32 Index = LastAvailableIndex + 1; Index < ActorPool.Num() && Index != FirstAvailableIndex; ++Index)
			{
				KRVCHECK(GetPooledComponentFromActor(ActorPool[Index])->IsNearActorActive());
				OutActors.Emplace(ActorPool[Index]);
			}
		}
		if (FirstAvailableIndex < LastAvailableIndex)
		{
			for (int32 Index = 0; Index < ActorPool.Num() && Index != FirstAvailableIndex; ++Index)
			{
				KRVCHECK(GetPooledComponentFromActor(ActorPool[Index])->IsNearActorActive());
				OutActors.Emplace(ActorPool[Index]);
			}
		}
		if (OutActors.Num() > 0)
		{
			return true;
		}
	}
	return false;

}

void USpawnerComponent::SetPool(TSubclassOf<AActor> NewPooledActorClass, int32 NewPoolSize)
{
	if (!NewPooledActorClass || !(NewPoolSize > 0) || !GetWorld())
	{
		KRVCHECK(NewPooledActorClass);
		KRVCHECK(NewPoolSize > 0);
		KRVCHECK(GetWorld());
		return;
	}
	PooledActorClass = NewPooledActorClass;
	ComponentPool.Empty();
	ActorPool.Empty();
	PoolSize = NewPoolSize;

	// spare 1 more space for algorithm
	// 실제 풀크기 보다 1개 더 배정
	ActorPool.Reserve(NewPoolSize);
	for (int32 i = 0; i < NewPoolSize; ++i)
	{
		UPooledComponent* SpawnedPooledComponent = InitPooledActor(i);
		if (!SpawnedPooledComponent)
		{
			KRVCHECK(SpawnedPooledComponent);
			continue;
		}
		else
		{
			ComponentPool.Emplace(SpawnedPooledComponent);
			ActorPool.Emplace(SpawnedPooledComponent->GetNearActor());
		}
	}
	FirstAvailableIndex = 0;
	LastAvailableIndex = NewPoolSize - 1;
	bIsPoolSet = true;

}

void USpawnerComponent::ExtendPool(int32 ExtendSize /*= 1*/)
{
	if (!PooledActorClass || ExtendSize <= 0 || !GetWorld())
	{
		KRVCHECK(PooledActorClass);
		KRVCHECK(ExtendSize > 0);
		KRVCHECK(GetWorld());
		return;
	}

	GetWorld()->GetTimerManager().PauseTimer(SpawnCooldownTimer);
	for (int32 Index = FirstAvailableIndex; Index < ActorPool.Num(); ++Index)
	{
		KRVCHECK(ActorPool[Index]);
		KRVCHECK(ComponentPool[Index]);
		ComponentPool[Index]->SetPoolIndex(ComponentPool[Index]->GetPoolIndex() + ExtendSize);
	}
	if (FirstAvailableIndex <= LastAvailableIndex)
	{
		LastAvailableIndex += ExtendSize;
	}
	PoolSize += ExtendSize;

	ComponentPool.InsertUninitialized(FirstAvailableIndex, ExtendSize);
	ActorPool.InsertUninitialized(FirstAvailableIndex, ExtendSize);
	for (int32 Index = FirstAvailableIndex; Index < FirstAvailableIndex + ExtendSize; ++Index)
	{
		ComponentPool[Index] = InitPooledActor(Index);
		ActorPool[Index] = ComponentPool[Index]->GetNearActor();
	}
	GetWorld()->GetTimerManager().UnPauseTimer(SpawnCooldownTimer);

}

void USpawnerComponent::DestroyPool()
{
	for (int32 Index = ActorPool.Num() - 1; Index >= 0; --Index)
	{
		ActorPool[Index]->ConditionalBeginDestroy();
	}
	ActorPool.Empty();
	ComponentPool.Empty();
}

UPooledComponent* USpawnerComponent::InitPooledActor(int32 PoolIndex)
{
	if (!GetWorld() || !PooledActorClass)
	{
		KRVCHECK(GetWorld());
		KRVCHECK(PooledActorClass);
		return nullptr;
	}
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	AActor* PooledActor = GetWorld()->SpawnActor(PooledActorClass, &FVector::ZeroVector, &FRotator::ZeroRotator, SpawnParameters);

	UPooledComponent* PooledComponent = GetPooledComponentFromActor(PooledActor);
	if (!PooledComponent)
	{
		KRVCHECK(PooledComponent);
		return nullptr;
	}
	PooledComponent->SetSpawnerComponent(this);
	PooledComponent->SetPoolIndex(PoolIndex);

	PooledActor->SetActorHiddenInGame(true);
	PooledActor->SetActorEnableCollision(false);
	UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(PooledActor->GetRootComponent());
	if (PrimitiveComponent)
	{
		PrimitiveComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
		PrimitiveComponent->SetEnableGravity(false);
	}
	return PooledComponent;

}

void USpawnerComponent::SetPoolWithCharacterSpawnData(const FKRVCharacterSpawnData& Data)
{
	if (!Data.CharacterClass)
	{
		KRVCHECK(Data.CharacterClass);
	}
	else
	{
		SetPool(Data.CharacterClass, Data.CharactersToSpawn + SpareCharacters);
		SetSpawnParams(FVector(200.f, 0.f, 0.f), FRotator::ZeroRotator,
			true, 1, Data.SpawnRate);
		SetBoxSize(FVector(100.f, 100.f, 0.f));
	}

}

void USpawnerComponent::SpawnRecursive()
{
	SpawnOnce();

	if (!bIsInLoop && --RepeatingCallsRemaining == 0)
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(SpawnCooldownTimer);
	}
	else
	{
		if (InRate == 0.f)
		{
			SpawnRecursive();
		}
		else
		{
			(GetOwner()->GetWorldTimerManager()).SetTimer(SpawnCooldownTimer, this, &USpawnerComponent::SpawnRecursive, InRate, false);
		}
	}

}

void USpawnerComponent::SetBoxSize(const FVector& NewSize)
{
	BoxSize = NewSize;

}

UPooledComponent* USpawnerComponent::GetPooledComponent()
{
	KRVLOG(Log, TEXT("First %d, Last %d"), FirstAvailableIndex, LastAvailableIndex);
	if (FirstAvailableIndex == LastAvailableIndex && ComponentPool[FirstAvailableIndex]->IsNearActorActive())
	{
		KRVLOG(Warning, TEXT("The pool is all in use"));
		return nullptr;
	}
	KRVCHECK(ComponentPool[FirstAvailableIndex]);
	UPooledComponent* ComponentToReturn = ComponentPool[FirstAvailableIndex];
	FirstAvailableIndex = (FirstAvailableIndex == PoolSize - 1) ? 0 : ++FirstAvailableIndex;
	return ComponentToReturn;

}

void USpawnerComponent::CheckOutDeactivatedObject(int32 IndexToBeDeactivated)
{
	if (IndexToBeDeactivated == -1)
	{
		// PooledComp has dummy SpawnerComp, just deactivate PooledActor itself.
		// -1 이면 풀드컴포넌트가 더미 스포너컴넌트를 가지고 있음. 여기선 아무것도 하지않음.
		return;
	}
	else
	{
		KRVLOG(Log, TEXT("IndexToBeDeactivated %d"), IndexToBeDeactivated);
		KRVCHECK(IndexToBeDeactivated >= 0 && IndexToBeDeactivated <= PoolSize);
		AActor* ActorToBeDeactivated = ActorPool[IndexToBeDeactivated];
		const UPooledComponent* const PooledComponent = GetPooledComponentFromActor(ActorToBeDeactivated);
		KRVCHECK(PooledComponent);
		KRVCHECK(PooledComponent->GetPoolIndex() == IndexToBeDeactivated);
		KRVCHECK(!(PooledComponent->IsNearActorActive()));

		LastAvailableIndex = (LastAvailableIndex == PoolSize - 1) ? 0 : ++LastAvailableIndex;
		KRVLOG(Log, TEXT("First %d, Last %d"), FirstAvailableIndex, LastAvailableIndex);
		if (ComponentPool[LastAvailableIndex]->IsNearActorActive())
		{
			ActorPool.Swap(LastAvailableIndex, IndexToBeDeactivated);
		}
	}

}


UPooledComponent* USpawnerComponent::GetPooledComponentFromActor(AActor* PooledActor) const
{
	TArray<UPooledComponent*> ReturnedPooledComponents;
	PooledActor->GetComponents<UPooledComponent>(ReturnedPooledComponents);
	if (ReturnedPooledComponents.Num() == 0)
	{
		KRVLOG(Warning, TEXT("The Actor does not have PooledComponent"));
		return nullptr;
	}
	return ReturnedPooledComponents[0];

}

AActor* USpawnerComponent::SpawnOnce()
{
	UPooledComponent* PooledComponent = GetPooledComponent();
	if (!PooledComponent)
	{
		KRVCHECK(PooledComponent);
	}
	else
	{
		AActor* ActorToBeSpawned = PooledComponent->GetNearActor();

		FVector SpawnWorldLocation;
		FRotator SpawnWorldRotation;
		if (bIsRelative)
		{
			SpawnWorldLocation = GetComponentLocation() + GetComponentRotation().RotateVector(SpawnLocation);
			SpawnWorldRotation = FRotator(FQuat(SpawnRotation) * FQuat(GetComponentRotation()));
		}
		else
		{
			SpawnWorldLocation = SpawnLocation;
			SpawnWorldRotation = SpawnRotation;
		}
		if (BoxSize != FVector::ZeroVector)
		{
			SpawnWorldLocation = UKismetMathLibrary::RandomPointInBoundingBox(
				SpawnWorldLocation, BoxSize);
		}
		KRVLOG(Log, TEXT("RandomPoint %s"), *(SpawnWorldLocation.ToString()));
		ActorToBeSpawned->SetActorLocation(SpawnWorldLocation);
		ActorToBeSpawned->SetActorRotation(SpawnWorldRotation);

		AKRVCharacter* CharacterSpawned = Cast<AKRVCharacter>(ActorToBeSpawned);
		if (CharacterSpawned)
		{
			OnSpawnCharacterDelegate.Broadcast(CharacterSpawned);
		}

		PooledComponent->SetNearActorLifeSpan(SpawnedActorLifeSpan);
		PooledComponent->SetNearActorActive(true);

		return ActorToBeSpawned;
	}
	return nullptr;

}


