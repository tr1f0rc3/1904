// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVBuff.h"
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"

#include "Engine/AssetManager.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

#include "KRV.h"
#include "KRVEnums.h"
#include "DataAssets/KRVBuffDataAsset.h"
#include "SceneComponents/PooledComponent.h"
#include "KRVGameInstance.h"
#include "GameModes/KRVInGameGameMode.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"

AKRVBuff::AKRVBuff()
{
	PrimaryActorTick.bCanEverTick = false;

	PooledComponent = CreateDefaultSubobject<UPooledComponent>(TEXT("POOLED_COMPONENT"));
	SetRootComponent(PooledComponent);

	bIsDataLoaded = false;
	bDoesBuffTaken = false;
	bDoesBuffParticlePlayed = false;
	bDoesBuffSoundPlayed = false;

}

void AKRVBuff::BeginPlay()
{
	Super::BeginPlay();

}

void AKRVBuff::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	LoadFromDataAsset_Implementation();

}

bool AKRVBuff::LoadFromDataAsset_Implementation()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	HardPtr_MyDataAsset = Cast<UKRVDataAsset>
		(Streamable.LoadSynchronous(MyDataAsset.ToSoftObjectPath(), true));
	if (!GetBuffDataAsset() || bIsDataLoaded)
	{
		KRVCHECK(!bIsDataLoaded);
		KRVCHECK(GetBuffDataAsset());
		return false;
	}
	else if (!LoadStatData(GetBuffDataAsset()->ClassFName))
	{
		KRVLOG(Error, TEXT("Data is not loaded, the buff name is not found"));
		return false;
	}

	BuffParticlePath = GetBuffDataAsset()->BuffParticle.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(BuffParticlePath, FStreamableDelegate::CreateUObject(this,
		&AKRVBuff::OnBuffParticleAsyncLoaded));

	bIsDataLoaded = true;
	return true;

}

UKRVDataAsset* AKRVBuff::GetDataAsset_Implementation() const
{
	return HardPtr_MyDataAsset;

}

UKRVBuffDataAsset* AKRVBuff::GetBuffDataAsset() const
{
	return Cast<UKRVBuffDataAsset>(GetDataAsset_Implementation());
}

AKRVBuff* AKRVBuff::LoadStatData(FName _Name)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		KRVCHECK(World);
		return nullptr;
	}
	UKRVGameInstance* const KRVGameInstance = Cast<UKRVGameInstance>(UGameplayStatics::GetGameInstance(World));
	if (!KRVGameInstance)
	{
		KRVCHECK(KRVGameInstance);
		return nullptr;
	}
	const FKRVBuffStatData* const LoadedStatData = KRVGameInstance->GetBuffStatData(_Name);
	if (!LoadedStatData)
	{
		KRVCHECK(LoadedStatData);
		return nullptr;
	}

	SpellLevel = LoadedStatData->SpellLevel;
	Duration = LoadedStatData->Duration;
	TargetType = LoadedStatData->TargetType;

	AttackStat.MinDamage = LoadedStatData->MinDamage;
	AttackStat.MaxDamage = LoadedStatData->MaxDamage;
	AttackStat.DamageScale = LoadedStatData->DamageScale;
	AttackStat.AttackRate = LoadedStatData->AttackRate;
	AttackStat.AttackRateScale = LoadedStatData->AttackRateScale;
	AttackStat.AttackRange = LoadedStatData->AttackRange;
	AttackStat.AttackRangeScale = LoadedStatData->AttackRangeScale;

	DefenceStat.MaxHealthScale = LoadedStatData->MaxHealthScale;
	DefenceStat.MaxHealth = LoadedStatData->MaxHealth;
	DefenceStat.HealthRegenInRest = LoadedStatData->HealthRegenInRest;
	DefenceStat.HealthRegenInRestScale = LoadedStatData->HealthRegenInRestScale;
	DefenceStat.HealthRegenInCombat = LoadedStatData->HealthRegenInCombat;
	DefenceStat.HealthRegenInCombatScale = LoadedStatData->HealthRegenInCombatScale;
	DefenceStat.Armor = LoadedStatData->Armor;
	DefenceStat.ArmorScale = LoadedStatData->ArmorScale;
	DefenceStat.MagicResistance = LoadedStatData->MagicResistance;
	DefenceStat.MagicResistanceScale = LoadedStatData->MagicResistanceScale;

	MovementSpeed = LoadedStatData->MovementSpeed;
	MovementSpeedScale = LoadedStatData->MovementSpeedScale;

	DOTDamageType = LoadedStatData->DOTDamageType;
	TotalOvertimeAmount = LoadedStatData->TotalOvertimeAmount;
	OvertimeTicks = LoadedStatData->OvertimeTicks;
	return this;

}

void AKRVBuff::OnBuffParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(BuffParticlePath);
	if (AssetPathPtr.IsValid())
	{
		BuffParticle = AssetPathPtr.Get();
		PlayBuffParticle();
	}

}

void AKRVBuff::OnBuffSoundAsyncLoaded()
{
	TSoftObjectPtr<USoundCue> AssetPathPtr(BuffSoundPath);
	if (AssetPathPtr.IsValid())
	{
		BuffSound = AssetPathPtr.Get();
		PlayBuffSound();
	}

}

AKRVBuff* AKRVBuff::AssignmentOperator(const AKRVBuff* const Other)
{
	if (Other)
	{
		AttackStat = Other->AttackStat;
		DefenceStat = Other->DefenceStat;
		MovementSpeed = Other->MovementSpeed;
		MovementSpeedScale = Other->MovementSpeedScale;
	}
	return this;

}

void AKRVBuff::PlayBuffParticle()
{
	if (!bDoesBuffTaken || bDoesBuffParticlePlayed || !BuffParticle || !BuffOwner)
	{
		KRVCHECK(BuffParticle);
		KRVCHECK(BuffOwner);
		return;
	}

	TArray<FName> ParticleSocketNames = GetBuffDataAsset()->ParticleSocketNames;
	if (ParticleSocketNames.Num() > 0)
	{
		for (auto& SocketName : ParticleSocketNames)
		{
			KRVCHECK(BuffOwner->GetMesh()->DoesSocketExist(SocketName));
			BuffParticleComponents.Emplace(UGameplayStatics::SpawnEmitterAttached(BuffParticle,
				BuffOwner->GetMesh(), SocketName, FVector::ZeroVector, FRotator::ZeroRotator, FVector(1.f),
				EAttachLocation::SnapToTarget));
		}
	}
	else
	{
		BuffParticleComponents.Emplace(UGameplayStatics::SpawnEmitterAttached(BuffParticle, BuffOwner->GetMesh()));
	}
	bDoesBuffParticlePlayed = true;

}

void AKRVBuff::PlayBuffSound()
{
	if (!BuffSound || !BuffOwner || bDoesBuffSoundPlayed)
	{
		KRVCHECK(BuffParticle);
		KRVCHECK(BuffOwner);
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(BuffOwner, BuffSound, BuffOwner->GetActorLocation(), 1.f);
	bDoesBuffSoundPlayed = true;

}

bool AKRVBuff::GiveThis(AKRVCombatUnit* NewBuffOwner, AKRVCombatUnit* NewBuffCaster)
{
	if (!bIsDataLoaded)
	{
		LoadFromDataAsset_Implementation();
	}

	if (!GetBuffDataAsset() || !GetWorld() || !NewBuffCaster || !NewBuffOwner)
	{
		KRVCHECK(GetBuffDataAsset());
		KRVCHECK(NewBuffCaster);
		KRVCHECK(NewBuffOwner);
		KRVCHECK(GetWorld());
		return false;
	}

	else
	{
		BuffCaster = NewBuffCaster;
		BuffOwner = NewBuffOwner;
		if (!NewBuffOwner->TakeBuffWithStatUpdate(this))
		{
			return false;
		}
		else
		{
			SetOwner(NewBuffOwner);
			RegisterAllComponents();
			bDoesBuffTaken = true;

			if (Duration > 0)
			{
				GetWorldTimerManager().SetTimer
				(BuffRunoutTimerHandle, this, &AKRVBuff::OnTimeUp, Duration, false);
			}
			else if (Duration == -1.f)
			{
				// Infinite Duration
			}
			else
			{
				OnTimeUp();
			}

			// DOT & HOT
			AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(NewBuffOwner);
			if ((TargetType == ETargetType::FriendlyCharacter || TargetType == ETargetType::HostileCharacter)
				&& KRVCharacter && TotalOvertimeAmount > 0 && OvertimeTicks > 0)
			{
				RemainingTicks = OvertimeTicks;
				AmountPerTick = TotalOvertimeAmount / OvertimeTicks;
				OvertimePeriod = (Duration - 0.1) / OvertimeTicks;
				GetWorldTimerManager().SetTimer
				(OvertimeTimerHandle, this, &AKRVBuff::ApplyOvertime, OvertimePeriod, false);
			}

			PlayBuffParticle();
			PlayBuffSound();
			return true;

		}
	}

}

AKRVCombatUnit* AKRVBuff::GetBuffCaster() const
{
	return BuffCaster.Get();

}

void AKRVBuff::RemoveThis()
{
	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(BuffRunoutTimerHandle);
		GetWorldTimerManager().ClearTimer(OvertimeTimerHandle);
		for (auto BuffParticleComponent : BuffParticleComponents)
		{
			if (BuffParticleComponent)
			{
				BuffParticleComponent->DeactivateSystem();
			}
		}
	}

	// remove this in combatunit
	MultiOnThisBuffRemoved.Broadcast();
	// set variables for next spawn
	if (GetBuffCaster())
	{
		SetOwner(GetBuffCaster());
	}
	bDoesBuffTaken = false;
	bDoesBuffParticlePlayed = false;
	bDoesBuffSoundPlayed = false;
	PooledComponent->DeactivateNearActor();
}

void AKRVBuff::OnTimeUp()
{
	if (OnThisBuffRemovedThisPtr.IsBound())
	{
		OnThisBuffRemovedThisPtr.Execute(this);
		OnThisBuffRemovedThisPtr.Unbind();
	}
	else
	{
		KRVCHECK(OnThisBuffRemovedThisPtr.IsBound());
		RemoveThis();
	}

}

void AKRVBuff::ApplyOvertime()
{
	switch (TargetType)
	{
	case ETargetType::FriendlyCharacter:
		KRVCHECK(Cast<AKRVCharacter>(BuffOwner));
		Cast<AKRVCharacter>(BuffOwner)->GetHealed(AmountPerTick);
		break;
	case ETargetType::HostileCharacter:
		UGameplayStatics::ApplyDamage(BuffOwner, AmountPerTick, GetBuffCaster()->GetController(), GetBuffCaster(), DOTDamageType);
		break;
	case ETargetType::AnyUnit:
		break;
	default:
		break;
	}

	KRVCHECK(GetWorld());
	if (GetWorld() && --RemainingTicks)
	{
		GetWorldTimerManager().SetTimer(OvertimeTimerHandle, this, &AKRVBuff::ApplyOvertime, OvertimePeriod, false);
	}

}


