// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVProjectile.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/KismetMathLibrary.h"

#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "GameModes/KRVInGameGameMode.h"
#include "DataAssets/KRVProjectileDataAsset.h"
#include "SceneComponents/PooledComponent.h"
#include "Pawns/KRVCharacter.h"
#include "Pawns/KRVTower.h"

AKRVProjectile::AKRVProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

	HitBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("HIT_BOX_COMPONENT"));
	SetRootComponent(HitBoxComponent);
	HitBoxComponent->InitBoxExtent(FVector(20.f));
	HitBoxComponent->SetCollisionProfileName("KRVProjectile");
	HitBoxComponent->OnComponentHit.AddDynamic(this, &AKRVProjectile::OnProjectileHit);

	PooledComponent = CreateDefaultSubobject<UPooledComponent>(TEXT("POOLED_COMPONENT"));
	PooledComponent->SetupAttachment(RootComponent);
	PooledComponent->OnNearActorActivateDelegate.AddUObject(this, &AKRVProjectile::OnSpawned);
	PooledComponent->OnNearActorDeactivateDelegate.AddUObject(this, &AKRVProjectile::OnDestroyed);

	BodyMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BODY_MESH_COMPONENT"));
	BodyMeshComponent->SetupAttachment(RootComponent);
	BodyMeshComponent->SetRelativeTransform
	(BodyMeshComponent->GetSocketTransform(HitBoxSocketName, ERelativeTransformSpace::RTS_Actor));

	HeadParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HEAD_PARTICLE_COMPONENT"));
	HeadParticleComponent->SetupAttachment(RootComponent);
	HeadParticleComponent->SetAutoActivate(false);

	TailParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TAIL_PARTICLE_COMPONENT"));
	TailParticleComponent->SetupAttachment(RootComponent);
	TailParticleComponent->SetAutoActivate(false);

	FlyAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("FLY_AUDIO_COMPONENT"));
	FlyAudioComponent->SetupAttachment(RootComponent);
	FlyAudioComponent->SetAutoActivate(false);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("MOVEMENT_COMPONENT"));
	MovementComponent->UpdatedComponent = RootComponent;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bInitialVelocityInLocalSpace = true;
	MovementComponent->bShouldBounce = false;
	MovementComponent->bIsHomingProjectile = false;
	MovementComponent->Velocity = FVector::ZeroVector;

}


UKRVDataAsset* AKRVProjectile::GetDataAsset_Implementation() const
{
	return HardPtr_MyDataAsset;

}

UKRVProjectileDataAsset* AKRVProjectile::GetProjectileDataAsset() const
{
	return Cast<UKRVProjectileDataAsset>(GetDataAsset_Implementation());

}

bool AKRVProjectile::LoadFromDataAsset_Implementation()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	HardPtr_MyDataAsset = Cast<UKRVProjectileDataAsset>
		(Streamable.LoadSynchronous(MyDataAsset.ToSoftObjectPath(), true));
	if (!GetProjectileDataAsset())
	{
		KRVCHECK(GetProjectileDataAsset());
	}
	else
	{
		SetActorScale3D(GetProjectileDataAsset()->BodyMeshScale * FVector::OneVector);
		MovementComponent->bRotationFollowsVelocity = GetProjectileDataAsset()->bRotationFollowsVelocity;

		BodyMeshPath = GetProjectileDataAsset()->BodyMesh.ToSoftObjectPath();
		Streamable.RequestAsyncLoad(BodyMeshPath, FStreamableDelegate::CreateUObject(this,
			&AKRVProjectile::OnBodyMeshAsyncLoaded));

		return true;
	}
	return false;

}


void AKRVProjectile::OnBodyMeshAsyncLoaded()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	TSoftObjectPtr<UStaticMesh> AssetPathPtr(BodyMeshPath);
	if (AssetPathPtr.IsValid())
	{
		BodyMeshComponent->SetStaticMesh(AssetPathPtr.Get());
		BodyMeshComponent->SetRelativeRotation(FQuat(GetProjectileDataAsset()->BodyRotation));
		BodyMeshComponent->SetRelativeLocation(-1.f * BodyMeshComponent->GetSocketTransform(HitBoxSocketName).GetLocation());
	}

	HeadParticlePath = GetProjectileDataAsset()->HeadParticle.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(HeadParticlePath, FStreamableDelegate::CreateUObject(this,
		&AKRVProjectile::OnHeadParticleAsyncLoaded));

	TailParticlePath = GetProjectileDataAsset()->TailParticle.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(TailParticlePath, FStreamableDelegate::CreateUObject(this,
		&AKRVProjectile::OnTailParticleAsyncLoaded));

	FlySoundPath = GetProjectileDataAsset()->FlySound.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(FlySoundPath, FStreamableDelegate::CreateUObject(this,
		&AKRVProjectile::OnFlySoundAsyncLoaded));

	HitParticlePath = GetProjectileDataAsset()->HitParticle.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(HitParticlePath, FStreamableDelegate::CreateUObject(this,
		&AKRVProjectile::OnHitParticleAsyncLoaded));

	HitSoundPath = GetProjectileDataAsset()->HitSound.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(HitSoundPath, FStreamableDelegate::CreateUObject(this,
		&AKRVProjectile::OnHitSoundAsyncLoaded));

	OnDestroyedParticlePath = GetProjectileDataAsset()->HitParticle.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(OnDestroyedParticlePath, FStreamableDelegate::CreateUObject(this,
		&AKRVProjectile::OnOnDestroyedParticleAsyncLoaded));

}

void AKRVProjectile::OnHeadParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(HeadParticlePath);
	if (AssetPathPtr.IsValid())
	{
		HeadParticleComponent->SetTemplate(AssetPathPtr.Get());
		HeadParticleComponent->AttachToComponent(BodyMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, HeadSocketName);
	}

}

void AKRVProjectile::OnTailParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(TailParticlePath);
	if (AssetPathPtr.IsValid())
	{
		TailParticleComponent->SetTemplate(AssetPathPtr.Get());
		TailParticleComponent->AttachToComponent(BodyMeshComponent, FAttachmentTransformRules::SnapToTargetIncludingScale, TailSocketName);
	}

}

void AKRVProjectile::OnFlySoundAsyncLoaded()
{
	TSoftObjectPtr<USoundCue> AssetPathPtr(FlySoundPath);
	if (AssetPathPtr.IsValid())
	{
		FlyAudioComponent->SetSound(AssetPathPtr.Get());
	}
}

void AKRVProjectile::OnHitParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(HitParticlePath);
	if (AssetPathPtr.IsValid())
	{
		HitParticle = AssetPathPtr.Get();
	}

}

void AKRVProjectile::OnHitSoundAsyncLoaded()
{
	TSoftObjectPtr<USoundCue> AssetPathPtr(HitSoundPath);
	if (AssetPathPtr.IsValid())
	{
		HitSound = AssetPathPtr.Get();
	}

}

void AKRVProjectile::OnOnDestroyedParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(OnDestroyedParticlePath);
	if (AssetPathPtr.IsValid())
	{
		OnDestroyedParticle = AssetPathPtr.Get();
	}

}

void AKRVProjectile::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	LoadFromDataAsset_Implementation();

}

// Called every frame
void AKRVProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CheckHitToFlyTarget();
	RotateBody(DeltaTime);
	CheckMineActivated(DeltaTime);

}

void AKRVProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bIsFlying)
	{
		StopProjectileMovement();

		if (GetProjectileDataAsset())
		{
			float DeadBodyLifeSpan = GetProjectileDataAsset()->DeadBodyLifeSpan;
			float ExplosionDelay = GetProjectileDataAsset()->TimeToExplode;

			if (GetProjectileDataAsset()->RangedMediumType != ERangedMediumType::Arrow)
			{
				if (ExplosionDelay <= 0.f || !GetWorld())
				{
					Explode();
				}
				else
				{
					GetWorldTimerManager().SetTimer(ExplosionTimer, this,
						&AKRVProjectile::Explode, ExplosionDelay, false);
				}
			}
			else
			{
				if (DeadBodyLifeSpan == 0.f || !GetWorld())
				{
					DestroyThis();
				}
				else
				{
					GetWorldTimerManager().SetTimer(DeadbodyLifeSpanTimer, this,
						&AKRVProjectile::DestroyThis, DeadBodyLifeSpan, false);
				}
			}
		}
	}

}

void AKRVProjectile::StopProjectileMovement()
{
	bIsFlying = false;
	MovementComponent->Velocity = FVector::ZeroVector;
	MovementComponent->bIsHomingProjectile = false;
	MovementComponent->SetUpdatedComponent(NULL);
	TailParticleComponent->DeactivateSystem();
	FlyAudioComponent->Stop();

}

void AKRVProjectile::CheckHitToFlyTarget()
{
	if (!bIsFlying || !MovementComponent->bIsHomingProjectile)
	{
		return;
	}
	else if (!FlyTarget.IsValid())
	{
		MovementComponent->bIsHomingProjectile = false;
	}
	else
	{
		AKRVCombatUnit* HardPtr_FlyTarget = FlyTarget.Get();
		if (HardPtr_FlyTarget->GetActionStatus() == EActionStatus::Dead)
		{
			MovementComponent->bIsHomingProjectile = false;
		}
		else if (HardPtr_FlyTarget->GetCapsuleComponent() &&
			GetSquaredDistanceTo(HardPtr_FlyTarget) <=
			FMath::Square(HardPtr_FlyTarget->GetCapsuleComponent()->GetScaledCapsuleRadius() + DistToHitToTarget))
		{
			SetActorEnableCollision(false);

			if (GetProjectileDataAsset()->RangedMediumType != ERangedMediumType::Mine)
			{
				// Damage
				AKRVCharacter* TargetCharacter = Cast<AKRVCharacter>(HardPtr_FlyTarget);
				if (TargetCharacter)
				{
					switch (TargetType)
					{
					case ETargetType::FriendlyCharacter:
					{
						TargetCharacter->GetHealed(RangedAttackStat.GetRandDamage());
						break;
					}
					case ETargetType::HostileCharacter:
					{
						AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(this);
						if (!KRVInGameGameMode || !RangedAttackStat.DamageType)
						{
							KRVCHECK(KRVInGameGameMode);
							KRVCHECK(RangedAttackStat.DamageType);
						}
						else
						{
							TArray<FHitResult> HitResults;
							const FName TraceTag("MyTraceTag");
							GetWorld()->DebugDrawTraceTag = TraceTag;
							FCollisionQueryParams CollisionParams;
							CollisionParams.TraceTag = TraceTag;

							GetWorld()->LineTraceMultiByChannel(HitResults, GetActorLocation(),
								HardPtr_FlyTarget->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel2, CollisionParams);
							for (FHitResult HitResult : HitResults)
							{
								if (Cast<AKRVCombatUnit>(HitResult.GetActor()) == HardPtr_FlyTarget)
								{
									KRVInGameGameMode->ApplyDamageAndPlayEffect(HitResult, RangedAttackStat.GetRandDamage(),
										Shooter.Get(), RangedAttackStat.DamageType);
									break;
								}
							}
						}
						break;
					}
					default:
						break;
					}

					OnProjectileHitDelegate.Broadcast(TargetCharacter, Shooter.Get());
				}

				// StopMovement
				StopProjectileMovement();
				float DeadBodyLifeSpan = GetProjectileDataAsset()->DeadBodyLifeSpan;

				// Set Explosion Timer
				if (GetProjectileDataAsset()->RangedMediumType == ERangedMediumType::Bomb && bShouldExplode)
				{
					float TimeToExplode = GetProjectileDataAsset()->TimeToExplode;
					if (TimeToExplode == 0)
					{
						Explode();
					}
					else if (TimeToExplode < DeadBodyLifeSpan)
					{
						GetWorldTimerManager().SetTimer(ExplosionTimer, this,
							&AKRVProjectile::Explode, GetProjectileDataAsset()->TimeToExplode, false);
					}
				}

				// Set Destroy Timer
				if (DeadBodyLifeSpan == 0)
				{
					DestroyThis();
				}
				else
				{
					AttachToActor(Cast<AActor>(FlyTarget.Get()), FAttachmentTransformRules::KeepWorldTransform);
					GetWorldTimerManager().SetTimer(DeadbodyLifeSpanTimer, this,
						&AKRVProjectile::DestroyThis, DeadBodyLifeSpan, false);
				}
			}
		}
	}

}

void AKRVProjectile::CheckMineActivated(float DeltaTime)
{
	if (GetProjectileDataAsset()->RangedMediumType == ERangedMediumType::Mine)
	{
		if (!bIsFlying && bShouldExplode)
		{
			CurrentTimeToCheckMineActivation += DeltaTime;
			if (CurrentTimeToCheckMineActivation > IntervalToCheckMineActivation)
			{
				CurrentTimeToCheckMineActivation -= IntervalToCheckMineActivation;
				TArray<FHitResult> OutResults;
				if (UKRVBlueprintFunctionLibrary::MultiRadialTrace(Shooter.Get(), OutResults, GetActorLocation(),
					DistToActivateMine, TargetType))
				{
					Explode();
				}
			}
		}
	}

}


void AKRVProjectile::DestroyThis()
{
	PooledComponent->DeactivateNearActor();

}

void AKRVProjectile::OnSpawned()
{
	MovementComponent->SetUpdatedComponent(RootComponent);
	MovementComponent->bIsHomingProjectile = true;
	// If TailParticleComponent is activated OnSpawned, the particle would be shown across whole screen, since rapid change of velocity.
	// 바로 TailParticleComponent를 OnSpawned에서 액티베이트하면, 급격한 속도 변화로 인해 파티클이 화면을 가로지르게 나타나게 됨.
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(OnDepartedTimer, this,
			&AKRVProjectile::OnDeparted, 0.1f, false);
	}

}

void AKRVProjectile::OnDestroyed()
{
	HeadParticleComponent->DeactivateSystem();

	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(DeadbodyLifeSpanTimer);
		GetWorldTimerManager().ClearTimer(ExplosionTimer);
		GetWorldTimerManager().ClearTimer(OnDepartedTimer);

		if (OnDestroyedParticle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnDestroyedParticle,
				GetActorTransform(), true);
		}
	}

	// Setting for next fly
	// 다음 생애 발사 전 값 초기화
	StopProjectileMovement();
	bShouldExplode = false;
	SetTargetUnit(nullptr);

}

void AKRVProjectile::OnDeparted()
{
	bIsFlying = true;
	if (GetProjectileDataAsset() && GetProjectileDataAsset()->RangedMediumType != ERangedMediumType::Arrow)
	{
		bShouldExplode = true;
	}

	FlyAudioComponent->Play();
	HeadParticleComponent->ActivateSystem();
	TailParticleComponent->ActivateSystem();

}

void AKRVProjectile::RotateBody(float DeltaSeconds)
{
	if (!bIsFlying || MovementComponent->bRotationFollowsVelocity)
	{
		// not implemented;
	}
	else if (!BodyMeshComponent || !GetProjectileDataAsset())
	{
		KRVCHECK(BodyMeshComponent);
		KRVCHECK(GetProjectileDataAsset());
	}
	else
	{
		FQuat QuatToAdd = FQuat(GetProjectileDataAsset()->FlyRotationVelocity * DeltaSeconds);
		BodyMeshComponent->AddRelativeRotation(QuatToAdd);
	}

}

void AKRVProjectile::SetTargetUnit(AKRVCombatUnit* NewTarget)
{
	if (!NewTarget)
	{
		KRVCHECK(NewTarget);
		return;
	}
	else
	{
		FlyTarget = NewTarget;
		MovementComponent->HomingTargetComponent = Cast<USceneComponent>(NewTarget->GetRootComponent());
		bHasTarget = true;
		if (Shooter.IsValid())
		{
			if (UKRVBlueprintFunctionLibrary::IsHostile(Shooter.Get(), NewTarget))
			{
				TargetType = ETargetType::HostileCharacter;
			}
			else if (UKRVBlueprintFunctionLibrary::IsFriendly(Shooter.Get(), NewTarget))
			{
				TargetType = ETargetType::FriendlyCharacter;
			}
		}
	}

}

void AKRVProjectile::Explode()
{
	AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(this);
	if (!KRVInGameGameMode || !Shooter.IsValid())
	{
		KRVCHECK(KRVInGameGameMode);
		KRVCHECK(Shooter.IsValid());
	}
	else if (bShouldExplode)
	{
		switch (TargetType)
		{
			// Heal ally with damage amount
		case ETargetType::FriendlyCharacter:
		{
			TArray<struct FHitResult> OutHits;
			UKRVBlueprintFunctionLibrary::MultiRadialTrace
			(Shooter.Get(), OutHits, GetActorLocation(), SplashAttackStat.AttackRange, TargetType);

			for (auto& OutHit : OutHits)
			{
				AKRVCharacter* AllyCharacter = Cast<AKRVCharacter>(OutHit.GetActor());
				if (AllyCharacter)
				{
					AllyCharacter->GetHealed(SplashAttackStat.GetRandDamage());
				}
			}
			break;
		}
		// Damage to hostile
		case ETargetType::HostileCharacter:
		{
			KRVInGameGameMode->ApplyRadialDamageAndPlayEffect(SplashAttackStat.MinDamage, SplashAttackStat.MaxDamage, GetActorLocation(), SplashAttackStat.AttackRange, Shooter.Get(), SplashAttackStat.DamageType);
			break;
		}
		default:
			break;
		}

		bShouldExplode = false;
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), HitParticle,
			GetActorTransform(), true);
		UGameplayStatics::PlaySoundAtLocation(this, HitSound, GetActorLocation(), 1.f);
	}

	DestroyThis();

}

UProjectileMovementComponent* AKRVProjectile::GetMovementComponent() const
{
	return MovementComponent;

}
