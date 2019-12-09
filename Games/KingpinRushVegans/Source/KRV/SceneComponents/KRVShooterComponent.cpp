// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVShooterComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Engine/AssetManager.h"
#include "Components/BoxComponent.h"

#include "KRV.h"
#include "Structs/KRVAttackStat.h"
#include "DataAssets/KRVShooterComponentDataAsset.h"
#include "DataAssets/KRVProjectileDataAsset.h"
#include "ActorComponents/KRVStatComponent.h"
#include "ActorComponents/KRVCharacterStatComponent.h"
#include "ActorComponents/KRVTowerStatComponent.h"
#include "Actors/KRVProjectile.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Pawns/KRVTower.h"

UKRVShooterComponent::UKRVShooterComponent()
{
	FireParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FIRE_PARTICLE_COMPONENT"));
	FireParticleComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	FireAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FIRE_AUDIO_COMPONENT"));
	FireAudioComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform);

	bIsRelative = true;
	SpawnLocation = FVector(20.f, 0.f, 0.f);
	SpawnedActorLifeSpan = 10.f;

	bIsDataAssetLoaded = false;

}

void UKRVShooterComponent::BeginPlay()
{
	Super::BeginPlay();

}

void UKRVShooterComponent::InitializeComponent()
{
	Super::InitializeComponent();
	LoadFromDataAsset_Implementation();

}

UKRVDataAsset* UKRVShooterComponent::GetDataAsset_Implementation() const
{
	return HardPtr_MyDataAsset;

}

UKRVShooterComponentDataAsset* UKRVShooterComponent::GetShooterComponentDataAsset() const
{
	return Cast<UKRVShooterComponentDataAsset>(GetDataAsset_Implementation());

}

bool UKRVShooterComponent::LoadFromDataAsset_Implementation()
{
	if (!GetShooterComponentDataAsset() || bIsDataAssetLoaded)
	{
		KRVCHECK(GetShooterComponentDataAsset());
		KRVCHECK(!bIsDataAssetLoaded);
		return false;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	UParticleSystem* LoadedParticle = Cast<UParticleSystem>
		(Streamable.LoadSynchronous(GetShooterComponentDataAsset()->FireParticle.ToSoftObjectPath(), true));
	FireParticleComponent->SetTemplate(LoadedParticle);

	USoundCue* LoadedSound = Cast<USoundCue>
		(Streamable.LoadSynchronous(GetShooterComponentDataAsset()->FireSound.ToSoftObjectPath(), true));
	FireAudioComponent->SetSound(LoadedSound);

	RelativeFireTransform = FTransform(GetShooterComponentDataAsset()->FireRotation, GetShooterComponentDataAsset()->FireLocation, FVector::ZeroVector);

	SetPool(GetShooterComponentDataAsset()->ProjectileClass, GetShooterComponentDataAsset()->MaxProjectileInAir);
	SetProjectileFlyParams(GetShooterComponentDataAsset()->InitialProjectileSpeed,
		GetShooterComponentDataAsset()->MaxProjectileSpeed, GetShooterComponentDataAsset()->HomingAcceleration,
		GetShooterComponentDataAsset()->GravityScale);
	SetSpawnParams(GetShooterComponentDataAsset()->FireLocation, GetShooterComponentDataAsset()->FireRotation, true);
	
	bIsDataAssetLoaded = true;
	return true;

}


bool UKRVShooterComponent::SetDataAssetAndLoad(UKRVShooterComponentDataAsset* NewDataAsset)
{
	HardPtr_MyDataAsset = Cast<UKRVDataAsset>(NewDataAsset);
	return LoadFromDataAsset_Implementation();

}

bool UKRVShooterComponent::SetShooter(AKRVCombatUnit* NewShooter)
{
	if (!NewShooter)
	{
		KRVCHECK(NewShooter);
		return false;
	}
	Shooter = NewShooter;
	for (auto PooledActor : ActorPool)
	{
		AKRVProjectile* KRVProjectile = Cast<AKRVProjectile>(PooledActor);
		KRVCHECK(KRVProjectile);
		if (KRVProjectile)
		{
			if (Shooter.IsValid())
			{
				KRVProjectile->Shooter = Shooter.Get();
			}
		}
	}
	return true;

}

AKRVCombatUnit* UKRVShooterComponent::GetTargetUnit() const
{
	if (FlyTarget.IsValid())
	{
		return FlyTarget.Get();
	}
	return nullptr;

}

void UKRVShooterComponent::SetTargetUnit(AKRVCombatUnit* NewTarget)
{
	FlyTarget = NewTarget;

}

AKRVProjectile* UKRVShooterComponent::Fire(const FKRVAttackStat& NewRangedAttackStat, const FKRVAttackStat& NewSplashAttackStat)
{
	AKRVProjectile* KRVProjectile = Cast<AKRVProjectile>(Spawn());
	KRVCHECK(KRVProjectile);
	if (KRVProjectile)
	{
		KRVProjectile->GetMovementComponent()->Velocity =
			KRVProjectile->GetMovementComponent()->InitialSpeed *
			UKismetMathLibrary::GetForwardVector(FRotator(FQuat(GetOwner()->GetActorRotation()) * FQuat(SpawnRotation)));

		if (GetTargetUnit())
		{
			KRVProjectile->SetTargetUnit(GetTargetUnit());
		}

		KRVProjectile->RangedAttackStat = NewRangedAttackStat;
		KRVProjectile->SplashAttackStat = NewSplashAttackStat;
	}
	else
	{
		KRVLOG(Warning, TEXT("All projectiles in air"));
	}

	FireParticleComponent->ActivateSystem();
	FireAudioComponent->Play();

	return KRVProjectile;

}

AKRVProjectile* UKRVShooterComponent::SpawnMine(const FVector& SpawnLoc, ETargetType TargetTypeToResponse, const FKRVAttackStat& NewSplashAttackStat)
{
	if (ActorPool.Num() == 0)
	{
		KRVCHECK(ActorPool.Num() > 0);
	}
	else
	{
		AKRVProjectile* MineToCheck = Cast<AKRVProjectile>(ActorPool[0]);

		bool bIsMine = false;
		if (!MineToCheck)
		{
			KRVCHECK(MineToCheck);
		}
		else if (!MineToCheck->GetProjectileDataAsset())
		{
			KRVCHECK(MineToCheck->GetProjectileDataAsset());
		}
		else if (MineToCheck->GetProjectileDataAsset()->RangedMediumType != ERangedMediumType::Mine)
		{
			KRVCHECK(MineToCheck->GetProjectileDataAsset()->RangedMediumType == ERangedMediumType::Mine);
		}
		else
		{
			bIsMine = true;
		}

		if (bIsMine)
		{
			AKRVProjectile* MineToSpawn = Cast<AKRVProjectile>(Spawn());
			if (!MineToSpawn)
			{
				KRVCHECK(MineToSpawn);
			}
			else
			{
				MineToSpawn->SplashAttackStat = NewSplashAttackStat;
				MineToSpawn->TargetType = TargetTypeToResponse;
				MineToSpawn->SetActorLocation(SpawnLoc + 50 * FVector::UpVector);
				MineToSpawn->GetMovementComponent()->Velocity = FVector::ZeroVector;

				return MineToSpawn;
			}
		}
	}
	return nullptr;

}

void UKRVShooterComponent::SetProjectileFlyParams(float _InitSpeed, float _MaxSpeed, float _Accel, float _GravityScale)
{
	for (auto PooledActor : ActorPool)
	{
		AKRVProjectile* KRVProjectile = Cast<AKRVProjectile>(PooledActor);
		KRVCHECK(KRVProjectile);
		if (KRVProjectile)
		{
			if (Shooter.IsValid())
			{
				KRVProjectile->Shooter = Shooter.Get();
			}
			UProjectileMovementComponent* MovementComponent = KRVProjectile->GetMovementComponent();
			MovementComponent->InitialSpeed = _InitSpeed;
			MovementComponent->MaxSpeed = _MaxSpeed;
			MovementComponent->HomingAccelerationMagnitude = _Accel;
			MovementComponent->ProjectileGravityScale = _GravityScale;
		}
	}

}

