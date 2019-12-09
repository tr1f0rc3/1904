// Fill out your copyright notice in the Description page of Project Settings.

#include "PooledComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

#include "KRV.h"
#include "SceneComponents/SpawnerComponent.h"
#include "Controllers/KRVAIController.h"

#include "Pawns/KRVCombatUnit.h"

UPooledComponent::UPooledComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;

	OnSpawnedAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ON_SPAWNED_AUDIO_COMPONENT"));
	OnSpawnedAudioComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	OnSpawnedAudioComponent->SetAutoActivate(false);

	OnSpawnedParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ON_SPAWNED_PARTICLE_COMPONENT"));
	OnSpawnedParticleComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);
	OnSpawnedParticleComponent->SetAutoActivate(false);


	PoolIndex = -1;
	OwnerLifeSpan = 0.f;

}

void UPooledComponent::InitializeComponent()
{
	SetNearActor();

}

USpawnerComponent* UPooledComponent::GetSpawnerComponent() const
{
	if (SpawnerComponent.IsValid())
	{
		return SpawnerComponent.Get();
	}
	else
	{
		return nullptr;
	}

}

void UPooledComponent::SetSpawnerComponent(USpawnerComponent* NewSpawnerComponent)
{
	if (NewSpawnerComponent)
	{
		SpawnerComponent = NewSpawnerComponent;
		bHasSpawnerComponent = true;
	}
	else
	{
		KRVCHECK(NewSpawnerComponent);
	}

}

int32 UPooledComponent::GetPoolIndex() const
{
	return PoolIndex;

}

void UPooledComponent::SetPoolIndex(int32 Index)
{
	PoolIndex = Index;

}

void UPooledComponent::SetNearActorLifeSpan(float NewLifeSpan)
{
	OwnerLifeSpan = NewLifeSpan;

}

void UPooledComponent::SetNearActorActive(bool IsActive)
{
	AActor* NearActor = GetNearActor();
	if (!NearActor)
	{
		KRVCHECK(NearActor);
		return;
	}

	bIsOwnerActive = IsActive;
	HideNearActor(!IsActive);

	if (IsActive)
	{
		OnNearActorActivateDelegate.Broadcast();
		OnSpawnedParticleComponent->ActivateSystem(true);
		OnSpawnedAudioComponent->Play();
		if (OwnerLifeSpan != 0.f)
		{
			NearActor->GetWorldTimerManager().SetTimer(LifeSpanTimer, this, &UPooledComponent::DeactivateNearActor, OwnerLifeSpan, false);
		}
	}
	else
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnDestroyedParticle,
			NearActor->GetActorLocation(), FRotator::ZeroRotator, FVector(1.f), true);
		OnNearActorDeactivateDelegate.Broadcast();
		if (bHasSpawnerComponent)
		{
			GetSpawnerComponent()->CheckOutDeactivatedObject(PoolIndex);
		}
		else
		{
			NearActor->GetWorldTimerManager().SetTimer(DestroyDelayTimer, this, &UPooledComponent::OnDestroyTimeUp, TimeToDestroy, false);
		}
	}

}

bool UPooledComponent::IsNearActorActive() const
{
	return bIsOwnerActive;

}

void UPooledComponent::HideNearActor(bool bIsTrue)
{
	if (GetNearActor())
	{
		GetNearActor()->SetActorHiddenInGame(bIsTrue);
		GetNearActor()->SetActorEnableCollision(!bIsTrue);
		GetNearActor()->SetActorTickEnabled(!bIsTrue);

		UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(GetNearActor()->GetRootComponent());
		if (PrimitiveComponent)
		{
			PrimitiveComponent->SetPhysicsLinearVelocity(FVector::ZeroVector);
			PrimitiveComponent->SetEnableGravity(!bIsTrue);
		}
	}

}

void UPooledComponent::OnDestroyTimeUp()
{
	AKRVCombatUnit* KRVCombatUnit = Cast<AKRVCombatUnit>(GetNearActor());
	if (KRVCombatUnit && KRVCombatUnit->MainAIController.IsValid())
	{
		KRVCombatUnit->MainAIController.Get()->ConditionalBeginDestroy();
	}

	TArray<USpawnerComponent*> SpawnerComponentsToDestroy;
	GetNearActor()->GetComponents<USpawnerComponent>(SpawnerComponentsToDestroy);
	for (USpawnerComponent* SpawnerComponentToDestroy : SpawnerComponentsToDestroy)
	{
		SpawnerComponentToDestroy->DestroyPool();
	}

	GetNearActor()->ConditionalBeginDestroy();

}

AActor* UPooledComponent::GetNearActor()
{
	return FoundNearActor;
}

void UPooledComponent::SetNearActor()
{
	UObject* RecursiveOuter = GetOuter();
	AActor* NearActor = nullptr;
	while (RecursiveOuter && !NearActor)
	{
		NearActor = Cast<AActor>(RecursiveOuter);
		RecursiveOuter = RecursiveOuter->GetOuter();
	}
	KRVCHECK(NearActor);
	if (NearActor)
	{
		FoundNearActor = NearActor;
	}

}

void UPooledComponent::DeactivateNearActor()
{
	AActor* NearActor = GetNearActor();
	if (!NearActor)
	{
		KRVCHECK(NearActor);
		return;
	}
	else
	{
		// force to deactivate before LifeSpan times up
		if (GetWorld() && LifeSpanTimer.IsValid())
		{
			NearActor->GetWorldTimerManager().ClearTimer(LifeSpanTimer);
		}
		SetNearActorActive(false);
	}

}

