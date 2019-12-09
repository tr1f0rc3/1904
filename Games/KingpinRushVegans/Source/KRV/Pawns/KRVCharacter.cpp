// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVCharacter.h"

#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Runtime/Engine/Public/WorldCollision.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

#include "Sound/SoundCue.h"
#include "Components/DecalComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SplineComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"

#include "KRV.h"
#include "KRVEnums.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Objects/KRVDamageType.h"
#include "DataAssets/KRVCharacterDataAsset.h"
#include "DataAssets/KRVWeaponDataAsset.h"
#include "DataAssets/KRVBuffDataAsset.h"

#include "Animations/KRVAnimInstance.h"
#include "SceneComponents/PooledComponent.h"
#include "SceneComponents/SpawnerComponent.h"
#include "ActorComponents/KRVCharacterStatComponent.h"
#include "Actors/KRVBuff.h"
#include "Actors/KRVProjectile.h"
#include "Controllers/KRVAIController.h"
#include "Controllers/KRVEnemyAIController.h"
#include "Controllers/KRVPlayerAIController.h"
#include "Controllers/KRVRunnerAIController.h"
#include "Pawns/KRVTower.h"
#include "GameModes/KRVInGameGameMode.h"

#include "UIs/KRVCombatUnitUserWidget.h"

AKRVCharacter::AKRVCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_SPAWNED_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Summon/P_Summon_Child_Startup.P_Summon_Child_Startup"));
	if (ON_SPAWNED_PARTICLE_OBJ.Succeeded())
	{
		PooledComponent->OnSpawnedParticleComponent->SetTemplate(ON_SPAWNED_PARTICLE_OBJ.Object);
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> ON_SPAWNED_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Teleport/Cues/retro_teleport_warp_effect_15_Cue.retro_teleport_warp_effect_15_Cue"));
	if (ON_SPAWNED_SOUND_OBJ.Succeeded())
	{
		PooledComponent->OnSpawnedAudioComponent->SetSound(ON_SPAWNED_SOUND_OBJ.Object);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_DESTROYED_PARTICLE_OBJ
	(TEXT("/Game/Resources/Particles/P_CharacterDestroyDust_1.P_CharacterDestroyDust_1"));
	if (ON_DESTROYED_PARTICLE_OBJ.Succeeded())
	{
		PooledComponent->OnDestroyedParticle = ON_DESTROYED_PARTICLE_OBJ.Object;
	}

	HealParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("HEAL_PRATICLE_COMPONENT"));
	HealParticleComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<UParticleSystem> HEAL_PARTICLE_OBJ
	(TEXT("/Game/Resources/Particles/P_Healed_01.P_Healed_01"));
	if (HEAL_PARTICLE_OBJ.Succeeded())
	{
		HealParticleComponent->SetTemplate(HEAL_PARTICLE_OBJ.Object);
		HealParticleComponent->SetAutoActivate(false);
	}

	HealAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("Heal_AUDIO_COMPONENT"));
	HealAudioComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USoundCue> HEAL_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Powerup/Cues/retro_powerup_collect_10_Cue.retro_powerup_collect_10_Cue"));
	if (HEAL_SOUND_OBJ.Succeeded())
	{
		HealAudioComponent->SetSound(HEAL_SOUND_OBJ.Object);
		HealAudioComponent->SetAutoActivate(false);
	}

	InvincibleParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("INVINCIBLE_PARTICLE_COMPONENT"));
	InvincibleParticleComponent->SetupAttachment(RootComponent);
	InvincibleParticleComponent->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> INVINCIBLE_PARTICLE_OBJ
	(TEXT("/Game/Resources/Particles/P_Invincible.P_Invincible"));
	if (INVINCIBLE_PARTICLE_OBJ.Succeeded())
	{
		InvincibleParticleComponent->SetTemplate(INVINCIBLE_PARTICLE_OBJ.Object);
	}

	InvincibleAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("INVINCIBLE_AUDIO_COMPONENT"));
	InvincibleAudioComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USoundCue> INVINCIBLE_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Magic/Cues/retro_magic_spell_cast_sparkle_06_Cue.retro_magic_spell_cast_sparkle_06_Cue"));
	if (INVINCIBLE_SOUND_OBJ.Succeeded())
	{
		InvincibleAudioComponent->SetSound(INVINCIBLE_SOUND_OBJ.Object);
		InvincibleAudioComponent->SetAutoActivate(false);
	}

	BaseCharacterStat = CreateDefaultSubobject<UKRVCharacterStatComponent>(TEXT("CHARACTER_BASE_STAT"));
	ModifiedCharacterStat = CreateDefaultSubobject<UKRVCharacterStatComponent>(TEXT("CHARACTER_MODIFIED_STAT"));

	// Melee
	MeleeRadiusDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("MELEE_RANGE_DECAL_COMPONENT"));
	MeleeRadiusDecal->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> MELEE_RANGE_MI_OBJ
	(TEXT("/Game/Resources/Materials/MAT_D_SelectionCircle_Orange.MAT_D_SelectionCircle_Orange"));
	if (MELEE_RANGE_MI_OBJ.Succeeded())
	{
		MeleeRadiusDecal->SetDecalMaterial(MELEE_RANGE_MI_OBJ.Object);
		MeleeRadiusDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		MeleeRadiusDecal->SetRelativeLocation(DecalLocation);
	}
	MeleeRadiusDecal->SetHiddenInGame(true);

	AggroParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("AGGRO_PARTICLE_COMPONENT"));
	AggroParticleComponent->SetupAttachment(GetMesh(), OverHeadSocket);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> AGGRO_PARTICLE_OBJ
	(TEXT("/Game/Resources/Particles/P_Aggro_1.P_Aggro_1"));
	if (AGGRO_PARTICLE_OBJ.Succeeded())
	{
		AggroParticleComponent->SetTemplate(AGGRO_PARTICLE_OBJ.Object);
		AggroParticleComponent->SetAutoActivate(false);
	}

	AggroAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("AGGRO_AUDIO_COMPONENT"));
	AggroAudioComponent->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<USoundCue> AGGRO_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Alarms_Sirens/Cues/retro_alarm_siren_loop_03_Cue.retro_alarm_siren_loop_03_Cue"));
	if (AGGRO_SOUND_OBJ.Succeeded())
	{
		AggroAudioComponent->SetSound(AGGRO_SOUND_OBJ.Object);
		AggroAudioComponent->SetAutoActivate(false);
	}

	LeftMeleeWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LEFT_MELEE_WEAPON"));
	LeftMeleeWeaponMesh->SetupAttachment(RootComponent);
	RightMeleeWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RIGHT_MELEE_WEAPON"));
	RightMeleeWeaponMesh->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<USoundCue> DEATH_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Damage_Hurt/Cues/retro_damage_hurt_ouch_28_Cue.retro_damage_hurt_ouch_28_Cue"));
	if (DEATH_SOUND_OBJ.Succeeded())
	{
		DeathAudioComponent->SetSound(DEATH_SOUND_OBJ.Object);
	}

}

void AKRVCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	CheckFallingAndRestoreLocation();

}

void AKRVCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	AKRVPlayerAIController* KRVPlayerAIController = Cast<AKRVPlayerAIController>(NewController);
	AKRVEnemyAIController* KRVEnemyAIController = Cast<AKRVEnemyAIController>(NewController);
	if (KRVPlayerAIController)
	{
		SetTeam(EKRVTeam::Player);
	}
	else if (KRVEnemyAIController)
	{
		SetTeam(EKRVTeam::Enemy);
	}

}

UKRVCharacterDataAsset* AKRVCharacter::GetCharacterDataAsset() const
{
	return Cast<UKRVCharacterDataAsset>(GetDataAsset_Implementation());

}

bool AKRVCharacter::LoadFromDataAsset_Implementation()
{
	if (!Super::LoadFromDataAsset_Implementation() || !GetCharacterDataAsset())
	{
		return false;
	}
	else
	{
		if (GetCharacterDataAsset()->bIsFlying)
		{
			DeadbodyLifeSpan = 0.3;
			GetCapsuleComponent()->SetCollisionProfileName("FlyingUnit");
			GetMesh()->AddWorldOffset(FlyHeight * FVector::UpVector);
		}
		else if (GetBaseCharacterStat()->MeleeAttackStat.MaxDamage > 0)
		{
			LoadMeleeWeapon(true);
			LoadMeleeWeapon(false);
		}
		return true;
	}

}

bool AKRVCharacter::LoadBaseStat()
{
	if (!GetDataAsset_Implementation() || !GetBaseCharacterStat() || !GetModifiedCharacterStat())
	{
		return false;
	}
	if (GetBaseCharacterStat()->LoadData(GetDataAsset_Implementation()->ClassFName) &&
		GetModifiedCharacterStat()->LoadData(GetDataAsset_Implementation()->ClassFName))
	{
		return true;
	}
	return false;

}

bool AKRVCharacter::LoadMeleeWeapon(bool bIsLeft, int32 UpgradeIndex)
{
	if (!GetCharacterDataAsset())
	{
		KRVCHECK(GetCharacterDataAsset());
		return false;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	if (bIsLeft && GetMesh()->DoesSocketExist(LeftHandSocket))
	{
		if (UpgradeIndex >= GetCharacterDataAsset()->LeftMeleeWeapon.Num() || UpgradeIndex < 0)
		{
			KRVLOG(Warning, TEXT("Not available index %d"), UpgradeIndex);
			return false;
		}
		UKRVWeaponDataAsset* LeftMeleeWeaponDataAsset = Cast<UKRVWeaponDataAsset>(Streamable.LoadSynchronous
		((GetCharacterDataAsset()->LeftMeleeWeapon)[UpgradeIndex].ToSoftObjectPath(), true));
		if (LeftMeleeWeaponDataAsset)
		{
			LeftMeleeWeaponMeshPath = LeftMeleeWeaponDataAsset->WeaponMesh.ToSoftObjectPath();
			LeftMeleeWeaponType = LeftMeleeWeaponDataAsset->WeaponType;
			Streamable.RequestAsyncLoad(LeftMeleeWeaponMeshPath, FStreamableDelegate::CreateUObject(this,
				&AKRVCharacter::OnLeftMeleeWeaponAsyncLoaded));
		}
	}
	else if (GetMesh()->DoesSocketExist(RightHandSocket))
	{
		if (UpgradeIndex >= GetCharacterDataAsset()->RightMeleeWeapon.Num() || UpgradeIndex < 0)
		{
			KRVLOG(Warning, TEXT("Not available index %d"), UpgradeIndex);
			return false;
		}
		UKRVWeaponDataAsset* RightMeleeWeaponDataAsset = Cast<UKRVWeaponDataAsset>(Streamable.LoadSynchronous
		((GetCharacterDataAsset()->RightMeleeWeapon)[UpgradeIndex].ToSoftObjectPath(), true));
		if (RightMeleeWeaponDataAsset)
		{
			RightMeleeWeaponMeshPath = RightMeleeWeaponDataAsset->WeaponMesh.ToSoftObjectPath();
			RightMeleeWeaponType = RightMeleeWeaponDataAsset->WeaponType;
			Streamable.RequestAsyncLoad(RightMeleeWeaponMeshPath, FStreamableDelegate::CreateUObject(this,
				&AKRVCharacter::OnRightMeleeWeaponAsyncLoaded));
		}
	}
	return true;

}

void AKRVCharacter::ShowMeleeRadiusDecal(bool bDoesShow)
{
	if (GetActionStatus() == EActionStatus::Dead)
	{
		return;
	}
	else if (MeleeRadiusDecal)
	{
		MeleeRadiusDecal->SetHiddenInGame(!bDoesShow);
	}

}

void AKRVCharacter::OnLeftMeleeWeaponAsyncLoaded()
{
	TSoftObjectPtr<UStaticMesh> AssetPathPtr(LeftMeleeWeaponMeshPath);
	if (AssetPathPtr.IsValid())
	{
		LeftMeleeWeaponMesh->SetStaticMesh(AssetPathPtr.Get());
		LeftMeleeWeaponMesh->SetCollisionProfileName("NoCollision");
		if (LeftMeleeWeaponType == EWeaponType::Shield)
		{
			LeftMeleeWeaponMesh->AttachToComponent
			(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackShieldSocket);
		}
		else
		{
			LeftMeleeWeaponMesh->AttachToComponent
			(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackLeftWeaponSocket);
		}
	}

}

void AKRVCharacter::OnRightMeleeWeaponAsyncLoaded()
{
	TSoftObjectPtr<UStaticMesh> AssetPathPtr(RightMeleeWeaponMeshPath);
	if (AssetPathPtr.IsValid())
	{
		RightMeleeWeaponMesh->SetStaticMesh(AssetPathPtr.Get());
		RightMeleeWeaponMesh->SetCollisionProfileName("NoCollision");
		RightMeleeWeaponMesh->AttachToComponent
		(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackRightWeaponSocket);
	}

}

void AKRVCharacter::OnDieMontageAsyncLoaded()
{
	Super::OnDieMontageAsyncLoaded();
	if (DieMontage)
	{
		DeadbodyLifeSpan = DieMontage->GetPlayLength();
	}

}

AKRVCombatUnit* AKRVCharacter::GetSpawnerOfThis() const
{
	if (PooledComponent)
	{
		USpawnerComponent* SpawnerComponent = PooledComponent->GetSpawnerComponent();
		if (!SpawnerComponent)
		{
			return nullptr;
		}
		else
		{
			AKRVCombatUnit* Spawner = Cast<AKRVCombatUnit>(SpawnerComponent->GetOwner());
			return Spawner;
		}
	}
	else
	{
		return nullptr;
	}

}

void AKRVCharacter::CheckFallingAndRestoreLocation()
{
	if (FMath::Abs(GetActorLocation().Z) > 2000.f)
	{
		SetLocationToSpawnedLocation();
	}

}

float AKRVCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	if (bIsInvincible)
	{
		return 0.f;
	}

	UKRVDamageType* KRVDamageType = Cast<UKRVDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	if (!KRVDamageType)
	{
		KRVCHECK(KRVDamageType);
		return 0.f;
	}
	float DamageReductionRate = 0;
	EDamageReductionType DamageReductionType = KRVDamageType->DamageReductionType;
	switch (DamageReductionType)
	{
	case EDamageReductionType::TrueDamage:
		DamageReductionRate = 0;
		break;
	case EDamageReductionType::Physical:
		DamageReductionRate = GetModifiedCharacterStat()->DefenceStat.Armor;
		break;
	case EDamageReductionType::Magic:
		DamageReductionRate = GetModifiedCharacterStat()->DefenceStat.MagicResistance;
		break;
	default:
		break;
	}
	DamageReductionRate = FMath::Clamp<int32>(DamageReductionRate, -50, 95);
	DamageReductionRate /= 100;
	DamageAmount *= (1 - DamageReductionRate);

	float RemainingHealth = GetModifiedCharacterStat()->LoseHealth(DamageAmount);
	if (RemainingHealth < KINDA_SMALL_NUMBER)
	{
		Die();
	}
	else if (DamageAmount > 0)
	{
		SetRecentlyHit(true);
	}
	return DamageAmount;

}

UKRVCharacterStatComponent* AKRVCharacter::GetBaseCharacterStat() const
{
	return BaseCharacterStat;

}

UKRVCharacterStatComponent* AKRVCharacter::GetModifiedCharacterStat() const
{
	return ModifiedCharacterStat;

}

void AKRVCharacter::OnGameEnd(EKRVTeam Winner)
{
	if (GetActionStatus() != EActionStatus::Dead)
	{
		StopMoving();
		Super::OnGameEnd(Winner);
	}

}

void AKRVCharacter::OnEscaped()
{
	if (GetTeam() == EKRVTeam::Enemy)
	{
		DestroyThis();
	}

}

void AKRVCharacter::BePossessedByAIController(EKRVTeam Team, bool bDoesPlayNow /*= true*/)
{
	switch (Team)
	{
	case EKRVTeam::Player:
	{
		AIControllerClass = AKRVPlayerAIController::StaticClass();
		break;
	}
	case EKRVTeam::Enemy:
	{
		if (GetCharacterDataAsset()->bIsFlying)
		{
			AIControllerClass = AKRVRunnerAIController::StaticClass();
		}
		else
		{
			AIControllerClass = AKRVEnemyAIController::StaticClass();
		}
		break;
	}
	default:
		break;
	}

	Super::BePossessedByAIController(Team, bDoesPlayNow);

}

void AKRVCharacter::BindToWidgetsOnBeginPlay()
{
	Super::BindToWidgetsOnBeginPlay();

	UKRVCombatUnitUserWidget* CastedCombatUnitWidget = Cast<UKRVCombatUnitUserWidget>(CombatUnitWidget->GetUserWidgetObject());
	KRVCHECK(CastedCombatUnitWidget);
	if (CastedCombatUnitWidget)
	{
		CastedCombatUnitWidget->BindToCharacter(this);
	}

}

void AKRVCharacter::OnSelected(bool bIsSelected)
{
	if (bIsCurrentlySelected != bIsSelected)
	{
		Super::OnSelected(bIsSelected);

		ShowMeleeRadiusDecal(bIsSelected);
	}

}

void AKRVCharacter::OnAnimFireNotified()
{
	Super::OnAnimFireNotified();

	switch (GetActionStatus())
	{
	case EActionStatus::Attacking:
	{
		if (GetCombatStatus() == ECombatStatus::Melee)
		{
			OnMeleeAttackFire();
			break;
		}
	}
	default:
		break;
	}

}

UKRVStatComponent* AKRVCharacter::VariateBaseStat(TSubclassOf<AKRVBuff> BuffClass)
{
	if (!GetBaseCharacterStat())
	{
		KRVCHECK(GetBaseCharacterStat());
		return nullptr;
	}
	Ptr_TemporaryBuff = NewObject<AKRVBuff>(this, BuffClass);
	Ptr_TemporaryBuff->SetOwner(this);
	*BaseCharacterStat += *Ptr_TemporaryBuff;
	Ptr_TemporaryBuff = nullptr;
	return GetBaseStat();

}

UKRVStatComponent* AKRVCharacter::GetBaseStat() const
{
	return Cast<UKRVStatComponent>(GetBaseCharacterStat());

}

UKRVStatComponent* AKRVCharacter::GetModifiedStat() const
{
	return Cast<UKRVStatComponent>(GetModifiedCharacterStat());

}

bool AKRVCharacter::TakeBuff(AKRVBuff* NewBuff)
{
	if (bIsInvincible && NewBuff->TargetType == ETargetType::HostileCharacter)
	{
		return false;
	}
	return Super::TakeBuff(NewBuff);

}

float AKRVCharacter::GetHealed(float HealAmount)
{
	HealParticleComponent->ActivateSystem();
	if (GetActionStatus() == EActionStatus::Dead || GetModifiedCharacterStat()->GetHealthRatio() == 1.f)
	{
		return -1.f;
	}
	float HealthBeforeHealing = GetModifiedCharacterStat()->GetHealth();
	float HealthAfterHealing = GetModifiedCharacterStat()->GainHealth(HealAmount);
	return HealthAfterHealing - HealthBeforeHealing;

}

void AKRVCharacter::HealthRegen()
{
	if (GetActionStatus() == EActionStatus::Dead)
	{
		return;
	}
	else if (bIsRecentlyHit)
	{
		GetModifiedCharacterStat()->GainHealth
		(GetModifiedCharacterStat()->DefenceStat.HealthRegenInCombat);
	}
	else
	{
		GetModifiedCharacterStat()->GainHealth
		(GetModifiedCharacterStat()->DefenceStat.HealthRegenInRest);
	}

	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(HealthRegenTimer, this, &AKRVCharacter::HealthRegen, 1.f, false);
	}

}

void AKRVCharacter::UpdateStat()
{
	UKRVCharacterStatComponent* CharacterBaseStat = GetBaseCharacterStat();
	UKRVCharacterStatComponent* CharacterModifiedStat = GetModifiedCharacterStat();
	if (!CharacterBaseStat || !CharacterModifiedStat)
	{
		KRVCHECK(CharacterBaseStat);
		KRVCHECK(CharacterModifiedStat);
		return;
	}
	if (CharacterModifiedStat && CharacterBaseStat)
	{
		CharacterModifiedStat->AssignmentOperator(CharacterBaseStat);
		*CharacterModifiedStat += *UpdateBuffStatSum();
		*CharacterModifiedStat *= *BuffSum;
		if (IsAbleToMove())
		{
			EnableWalk(true);
		}

		FireRadiusDecal->DecalSize.Y = GetModifiedCharacterStat()->RangedAttackStat.AttackRange;
		FireRadiusDecal->DecalSize.Z = GetModifiedCharacterStat()->RangedAttackStat.AttackRange;

		MeleeRadiusDecal->DecalSize.Y = TauntRange;
		MeleeRadiusDecal->DecalSize.Z = TauntRange;
	}
	OnStatUpdatedDelegate.Broadcast(this);

}

bool AKRVCharacter::MakeStunned(float NewStunDuration)
{
	if (bIsInvincible || !Super::MakeStunned(NewStunDuration))
	{
		return false;
	}
	return true;

}

bool AKRVCharacter::IsAbleToAttack() const
{
	if (!Super::IsAbleToAttack())
	{
		return false;
	}
	return true;

}

bool AKRVCharacter::SetFollowingPathStatus(bool bIsFollowingPath /*= true*/)
{
	if (!IsAbleToMove())
	{
		return false;
	}
	else if (GetCombatStatus() == ECombatStatus::Melee || GetCombatStatus() == ECombatStatus::Ranged ||
		GetCombatStatus() == ECombatStatus::ForcedMoving)
	{
		return false;
	}
	else if (bIsFollowingPath)
	{
		SetActionStatus(EActionStatus::Moving);
		SetCombatStatus(ECombatStatus::FollowingPath);
		return true;
	}
	else
	{
		SetActionStatus(EActionStatus::Idle);
		SetCombatStatus(ECombatStatus::Idle);
		return true;
	}

}

void AKRVCharacter::ResetStatus()
{
	Super::ResetStatus();

	bCanBeDamaged = true;
	EnableWalk(true);
	bIsInvincible = false;
	SetRecentlyHit(false);

	GetModifiedCharacterStat()->InitHealth();
	HealthRegen();

}

bool AKRVCharacter::StopAction()
{
	if (!Super::StopAction())
	{
		return false;
	}
	else
	{
		return true;
	}

}

void AKRVCharacter::OnStunTimeUp()
{
	Super::OnStunTimeUp();
	if (GetActionStatus() != EActionStatus::Dead)
	{
		EnableWalk(true);
	}

}

void AKRVCharacter::ClearAllActiveActionTimer()
{
	if (GetWorld())
	{
		Super::ClearAllActiveActionTimer();
		GetWorldTimerManager().ClearTimer(MeleeAttackRateTimer);
		GetWorldTimerManager().ClearTimer(MoveSuccessCheckTimer);
	}

}

void AKRVCharacter::ClearAllPassiveActionTimer()
{
	if (GetWorld())
	{
		Super::ClearAllPassiveActionTimer();
		GetWorldTimerManager().ClearTimer(InvincibleTimer);
		GetWorldTimerManager().ClearTimer(HealthRegenTimer);
		GetWorldTimerManager().ClearTimer(RecentlyHitTimer);
	}

}

float AKRVCharacter::GetInvincibleRemainingTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(StunTimer);

}

bool AKRVCharacter::BeInvincible(float NewDuration)
{
	if (GetActionStatus() == EActionStatus::Dead || NewDuration <= GetInvincibleRemainingTime())
	{
		return false;
	}
	else
	{
		if (GetActionStatus() == EActionStatus::Stunned)
		{
			OnStunTimeUp();
		}

		bIsInvincible = true;
		RemoveAllBuffsWithStatUpdate(ETargetType::HostileCharacter);

		InvincibleParticleComponent->ActivateSystem();
		InvincibleAudioComponent->Play();

		GetWorldTimerManager().ClearTimer(InvincibleTimer);
		GetWorldTimerManager().SetTimer(InvincibleTimer, this, &AKRVCharacter::OnInvincibleTimeUp,
			NewDuration, false);
		return true;
	}
	return false;

}

ECombatStatus AKRVCharacter::FindNearestEnemyRange(class AKRVCharacter*& OutCharacter, ECombatStatus StartRange /*= ECombatStatus::Idle*/)
{
	if (!GetBaseStat() || !GetModifiedStat())
	{
		KRVCHECK(GetBaseStat());
		KRVCHECK(GetModifiedStat());
		return ECombatStatus::NONE;
	}
	else
	{
		TArray<AKRVCharacter*> TargetsToCheck;
		ECombatStatus ReturnValue = ECombatStatus::Idle;

		if (StartRange == ECombatStatus::Melee)
		{
			if (FindCharactersToMeleeWithTurn(TargetsToCheck))
			{
				ReturnValue = ECombatStatus::Melee;
			}
			else
			{
				StartRange = ECombatStatus::Ranged;
			}
		}

		if (TargetsToCheck.Num() > 0)
		{
			OutCharacter = FindTheNearestCharacter(TargetsToCheck);
			OnNearestEnemyRangeChangedDelegate.Broadcast(ReturnValue, this);
			return ReturnValue;
		}
		else
		{
			return Super::FindNearestEnemyRange(OutCharacter, StartRange);
		}
	}

}

void AKRVCharacter::SetRecentlyHit(bool NewHit)
{
	if (!GetWorld())
	{
		KRVCHECK(GetWorld());
	}
	bIsRecentlyHit = NewHit;
	if (NewHit)
	{
		GetWorldTimerManager().SetTimer(RecentlyHitTimer, this,
			&AKRVCharacter::OnRecentlyHitTimeUp, RecentlyHitRecoveryTime, false);
	}
	else
	{
		GetWorldTimerManager().ClearTimer(RecentlyHitTimer);
	}

}

void AKRVCharacter::OnRecentlyHitTimeUp()
{
	SetRecentlyHit(false);

}

bool AKRVCharacter::DoReservedAction()
{
	if (!Super::DoReservedAction())
	{
		return false;
	}
	else
	{
		switch (NextActionToDo.ActionStatus)
		{
		case EActionStatus::Attacking:
		{
			if (NextActionToDo.CombatStatus == ECombatStatus::Melee)
			{
				MeleeAttack();
			}
			break;
		}
		case EActionStatus::Moving:
		{
			if (NextActionToDo.TargetUnit.IsValid())
			{
				MoveToTargetUnit();
			}
			else if (NextActionToDo.TargetPoint != FVector::ZeroVector)
			{
				MoveToTargetPoint(NextActionToDo.CombatStatus);
			}
			break;
		}
		break;
		case EActionStatus::MAX:
			break;
		default:
			break;
		}
		return true;
	}

}

void AKRVCharacter::OnInvincibleTimeUp()
{
	bIsInvincible = false;
	InvincibleParticleComponent->DeactivateSystem();

}

bool AKRVCharacter::MoveToTargetUnit()
{
	if (!GetTargetUnit() || !IsAbleToMove())
	{
		return false;
	}
	else
	{
		UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), GetTargetUnit());
		SetActionStatus(EActionStatus::Moving);
		if (UKRVBlueprintFunctionLibrary::IsSuitableTarget(GetTargetUnit(), Cast<AKRVCombatUnit>(this), ETargetType::HostileCharacter))
		{
			SetCombatStatus(ECombatStatus::Melee);
		}
		else
		{
			SetCombatStatus(ECombatStatus::ForcedMoving);
		}

		CheckMoveToTargetUnitSucceded();
		return true;
	}
	return false;

}

bool AKRVCharacter::MoveToTargetPoint(ECombatStatus NewStatus /*= ECombatStatus::FollowingPath*/)
{
	if (!(NewStatus == ECombatStatus::FollowingPath || NewStatus == ECombatStatus::ForcedMoving))
	{
		KRVCHECK(NewStatus == ECombatStatus::FollowingPath || NewStatus == ECombatStatus::ForcedMoving);
		return false;
	}
	else if (!IsAbleToMove())
	{
		return false;
	}
	else
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetTargetPoint());
		SetActionStatus(EActionStatus::Moving);
		SetCombatStatus(NewStatus);

		CheckMoveToTargetPointSucceded();
		return true;
	}
	return false;

}

bool AKRVCharacter::FollowPath(const FVector& NewPoint)
{
	if (GetCombatStatus() != ECombatStatus::FollowingPath &&
		GetActionStatus() != EActionStatus::Moving)
	{
		KRVCHECK(GetCombatStatus() == ECombatStatus::FollowingPath);
		KRVCHECK(GetActionStatus() == EActionStatus::Moving);
		return false;
	}
	else if (!IsAbleToMove())
	{
		return false;
	}
	else
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), NewPoint);
		return true;
	}
	return false;

}

bool AKRVCharacter::StopMoving()
{
	if (!IsAbleToMove())
	{
		return false;
	}
	else
	{
		UAIBlueprintHelperLibrary::SimpleMoveToLocation(GetController(), GetActorLocation());

		if (GetCombatStatus() == ECombatStatus::FollowingPath || GetCombatStatus() == ECombatStatus::ForcedMoving)
		{
			SetCombatStatus(ECombatStatus::Idle);
		}
		SetActionStatus(EActionStatus::Idle);
		return true;
	}

}

void AKRVCharacter::EnableWalk(bool bIsEnable)
{
	if (bIsEnable)
	{
		GetCharacterMovement()->MaxWalkSpeed = GetModifiedCharacterStat()->MovementSpeed;
	}
	else
	{
		GetCharacterMovement()->MaxWalkSpeed = 0;
	}

}

AKRVPath* AKRVCharacter::GetPathToFollow() const
{
	if (PathToFollow.IsValid())
	{
		return PathToFollow.Get();
	}
	return nullptr;

}

EPathFollowingRequestResult::Type AKRVCharacter::MoveToTargetUnitWithAI(AKRVCombatUnit* TargetUnit)
{
	AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(this);
	if (!TargetUnit || !AIController)
	{
		return EPathFollowingRequestResult::Failed;
	}
	else
	{
		if (Cast<AKRVCharacter>(TargetUnit) && IsHostile(TargetUnit))
		{
			return AIController->MoveToActor(TargetUnit, GetMeleeAttackableDistance() - 50.f, false);
		}
		else
		{
			return AIController->MoveToActor(TargetUnit, 50.f, true);
		}
	}

}

EPathFollowingRequestResult::Type AKRVCharacter::MoveToTargetPointWithAI(const FVector& NewPoint)
{
	AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(this);
	if (AIController)
	{
		return AIController->MoveToLocation(NewPoint);
	}
	else
	{
		return EPathFollowingRequestResult::Failed;
	}

}

void AKRVCharacter::CheckMoveToTargetUnitSucceded()
{
	RemainingMoveSuccessCheck = int32(MoveSuccessCheckDuration / MoveToUnitSuccessCheckInteval) + 1;
	CheckMoveToTargetUnitSucceded_RecursiveCheck();

}

void AKRVCharacter::CheckMoveToTargetUnitSucceded_RecursiveCheck()
{
	bool bShouldUpdateAgain = true;

	if (GetWorld())
	{
		if (GetCombatStatus() == ECombatStatus::Melee)
		{
			if (GetHorizontalDistanceTo(GetTargetUnit()) <= GetMeleeAttackableDistance())
			{
				bShouldUpdateAgain = false;
			}
		}
		else if (GetHorizontalDistanceTo(GetTargetUnit()) <= 50.f)
		{
			bShouldUpdateAgain = false;
		}

		if (bShouldUpdateAgain && --RemainingMoveSuccessCheck > 0)
		{
			GetWorldTimerManager().SetTimer(MoveSuccessCheckTimer, this,
				&AKRVCharacter::CheckMoveToTargetUnitSucceded_RecursiveCheck, MoveToUnitSuccessCheckInteval, false);
		}
		else
		{
			RemainingMoveSuccessCheck = 0;
			GetWorldTimerManager().ClearTimer(MoveSuccessCheckTimer);
			StopMoving();
			if (GetCombatStatus() == ECombatStatus::Melee)
			{
				MeleeAttack();
			}
		}
	}

}

void AKRVCharacter::CheckMoveToTargetPointSucceded()
{
	RemainingMoveSuccessCheck = int32(MoveSuccessCheckDuration / MoveToPointSuccessCheckInteval) + 1;
	CheckMoveToTargetPointSucceded_RecursiveCheck();

}

void AKRVCharacter::CheckMoveToTargetPointSucceded_RecursiveCheck()
{
	bool bShouldUpdateAgain = true;

	if (GetWorld())
	{
		if (FVector::DistSquared2D(GetActorLocation(), GetTargetPoint()) <= 2500.f)
		{
			bShouldUpdateAgain = false;
		}

		if (bShouldUpdateAgain && --RemainingMoveSuccessCheck > 0)
		{
			GetWorldTimerManager().SetTimer(MoveSuccessCheckTimer, this,
				&AKRVCharacter::CheckMoveToTargetPointSucceded_RecursiveCheck, MoveToPointSuccessCheckInteval, false);
		}
		else
		{
			RemainingMoveSuccessCheck = 0;
			GetWorldTimerManager().ClearTimer(MoveSuccessCheckTimer);
			StopMoving();
		}
	}

}


bool AKRVCharacter::Die()
{
	if (Super::Die())
	{
		bCanBeDamaged = false;

		EnableWalk(false);
		bIsInvincible = false;
		bIsRecruitedByTower = false;

		if (GetModifiedCharacterStat())
		{
			GetModifiedCharacterStat()->SetHealth(0.f);
		}
		if (GetBaseCharacterStat())
		{
			if (GetTeam() == EKRVTeam::Enemy)
			{
				OnDieWithRewardDelegate.Broadcast(GetBaseCharacterStat()->Bounty, GetBaseCharacterStat()->ExpGiven, Ruby, Cast<AKRVCombatUnit>(this));
			}
		}
		return true;
	}
	return false;

}

bool AKRVCharacter::WeaponSwap(ECombatStatus NewStatus)
{
	if (!IsAbleToMove())
	{
		KRVCHECK(IsAbleToMove());
		return false;
	}
	else if (!(NewStatus == ECombatStatus::Melee || NewStatus == ECombatStatus::Ranged))
	{
		KRVCHECK(NewStatus == ECombatStatus::Melee || NewStatus == ECombatStatus::Ranged);
		return false;
	}
	else if (CurrentWeaponRangeType == NewStatus)
	{
		return true;
	}
	else
	{
		CurrentWeaponRangeType = NewStatus;

		switch (NewStatus)
		{
		case ECombatStatus::Melee:
		{
			LeftRangedWeaponMesh->AttachToComponent
			(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackLeftWeaponSocket);
			RightRangedWeaponMesh->AttachToComponent
			(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackRightWeaponSocket);
			if (LeftMeleeWeaponType == EWeaponType::Shield)
			{
				LeftMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftShieldSocket);
			}
			else
			{
				LeftMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandSocket);
			}
			if (RightMeleeWeaponType == EWeaponType::Shield)
			{
				RightMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightShieldSocket);
			}
			else
			{
				RightMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocket);
			}
			break;
		}
		case ECombatStatus::Ranged:
		{
			LeftRangedWeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandSocket);
			RightRangedWeaponMesh->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocket);
			if (LeftMeleeWeaponType == EWeaponType::Shield)
			{
				LeftMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackShieldSocket);
			}
			else
			{
				LeftMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackLeftWeaponSocket);
			}
			if (RightMeleeWeaponType == EWeaponType::Shield)
			{
				RightMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackShieldSocket);
			}
			else
			{
				RightMeleeWeaponMesh->AttachToComponent
				(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackRightWeaponSocket);
			}
			break;
		}
		default:
			break;
		}

		if (GetKRVAnimInstance())
		{
			PlayWeaponSwapMontage(GetCurrentWeaponType());
		}

		return true;
	}
	return false;

}


ECombatStatus AKRVCharacter::GetAttackRangeType() const
{
	return CurrentWeaponRangeType;

}

bool AKRVCharacter::Fire()
{
	WeaponSwap(ECombatStatus::Ranged);
	if (Super::Fire())
	{
		EnableWalk(false);
		return true;
	}
	else
	{
		return false;
	}

}

bool AKRVCharacter::IsAbleToFire() const
{
	if (GetCombatStatus() == ECombatStatus::Melee)
	{
		return false;
	}
	return Super::IsAbleToFire();

}


void AKRVCharacter::OnAttackCooldownTimeUp()
{
	Super::OnAttackCooldownTimeUp();
	if (IsAbleToMove())
	{
		EnableWalk(true);
	}

}

void AKRVCharacter::OnAttackStatusTimeUp()
{
	if (GetCombatStatus() == ECombatStatus::Melee)
	{
		if (GetActionStatus() == EActionStatus::Stunned || GetActionStatus() == EActionStatus::Casting)
		{
			GetWorldTimerManager().SetTimer(AttackStatusTimer, this, &AKRVCharacter::OnAttackStatusTimeUp, AttackStatusDuration);
		}
		else
		{
			SetCombatStatus(ECombatStatus::Idle);
			WeaponSwap(ECombatStatus::Ranged);
		}
	}
	else
	{
		Super::OnAttackStatusTimeUp();
	}
}

float AKRVCharacter::GetAttackRemainingTime()
{
	if (!GetWorld())
	{
		return -1.f;
	}
	else
	{
		float RemainingMeleeAttackTime = GetWorldTimerManager().GetTimerRemaining(MeleeAttackRateTimer);
		return FMath::Max3(Super::GetAttackRemainingTime(), RemainingMeleeAttackTime, -1.f);
	}

}

EWeaponType AKRVCharacter::GetCurrentWeaponType() const
{
	if (CurrentWeaponRangeType == ECombatStatus::Melee)
	{
		return RightMeleeWeaponType;
	}
	else
	{
		return Super::GetCurrentWeaponType();
	}

}

bool AKRVCharacter::CastSkill(const FGameplayTag& SkillTagToCast, AKRVCombatUnit* NewUnit /*= nullptr*/, const FVector& NewPoint /*= FVector::ZeroVector*/)
{
	ECombatStatus LastCombatStatus = GetCombatStatus();
	if (Super::CastSkill(SkillTagToCast, NewUnit, NewPoint))
	{
		if (LastCombatStatus == ECombatStatus::Melee)
		{
			SetCombatStatus(ECombatStatus::Melee);
		}

		EnableWalk(false);
		return true;
	}
	else
	{
		return false;
	}

}

void AKRVCharacter::OnSkillCasted()
{
	Super::OnSkillCasted();

}

bool AKRVCharacter::GetAggro()
{
	if (!GetTargetUnit())
	{
		return false;
	}
	else if (!IsAbleToMove() || GetCombatStatus() == ECombatStatus::ForcedMoving)
	{
		return false;
	}
	else if (GetTargetUnit()->GetTargetUnit() == this ||
		GetTargetUnit()->GetCombatStatus() == ECombatStatus::Melee)
	{
		return false;
	}
	else if (!ChargeToMelee())
	{
		return false;
	}
	else
	{
		AKRVCharacter* CharacterToCharge = Cast<AKRVCharacter>(GetTargetUnit());
		KRVCHECK(CharacterToCharge);
		if (CharacterToCharge)
		{
			CharacterToCharge->SetTargetUnit(this);
			CharacterToCharge->ChargeToMelee();
			CharacterToCharge->AggroParticleComponent->ActivateSystem();
			CharacterToCharge->AggroAudioComponent->Play();
			return true;
		}
	}
	return false;

}

bool AKRVCharacter::IsInChargeRange(AKRVCharacter* CharacterToCharge /*= nullptr*/) const
{
	if (!CharacterToCharge)
	{
		CharacterToCharge = Cast<AKRVCharacter>(GetTargetUnit());
	}

	if (!CharacterToCharge)
	{
		KRVCHECK(CharacterToCharge);
		return false;
	}
	else if (!UKRVBlueprintFunctionLibrary::IsSuitableTarget
	(CharacterToCharge, this, ETargetType::HostileCharacter) ||
		CharacterToCharge->GetCharacterDataAsset()->bIsFlying ||
		CharacterToCharge->GetActionStatus() == EActionStatus::Dead)
	{
		return false;
	}
	else if (CharacterToCharge->GetCombatStatus() == ECombatStatus::Melee &&
		CharacterToCharge->GetTargetUnit() != Cast<AKRVCombatUnit>(this))
	{
		return false;
	}
	else if (CharacterToCharge->GetCombatStatus() == ECombatStatus::ForcedMoving)
	{
		return false;
	}
	else if (GetHorizontalDistanceTo(CharacterToCharge) > TauntRange)
	{
		return false;
	}
	else if (!CanSeeTheUnitWithTurn(CharacterToCharge))
	{
		return false;
	}
	else
	{
		return true;
	}
	return false;

}

bool AKRVCharacter::ChargeToMelee()
{
	if (!IsInChargeRange())
	{
		ClearTargetUnit();
		return false;
	}
	else
	{
		WeaponSwap(ECombatStatus::Melee);
		MoveToTargetUnit();
		AKRVCharacter* CharacterToCharge = Cast<AKRVCharacter>(GetTargetUnit());
		if (CharacterToCharge && CharacterToCharge->GetTargetUnit() != Cast<AKRVCombatUnit>(this))
		{
			CharacterToCharge->SetTargetUnit(Cast<AKRVCombatUnit>(this));
			CharacterToCharge->StopMoving();
			CharacterToCharge->MoveToTargetUnit();
		}
		return true;
	}
	return false;

}

float AKRVCharacter::GetMeleeAttackableDistance() const
{
	if (!GetModifiedCharacterStat() || !GetTargetUnit())
	{
		return -1.f;
	}
	else
	{
		return GetModifiedCharacterStat()->MeleeAttackStat.AttackRange +
			GetCapsuleComponent()->GetScaledCapsuleRadius() +
			GetTargetUnit()->GetCapsuleComponent()->GetScaledCapsuleRadius() + 50.f;
	}

}

bool AKRVCharacter::FindCharactersToMeleeWithTurn(TArray<AKRVCharacter*>& OutEnemies) const
{
	OutEnemies.Empty();
	TArray<FHitResult> OutHits;
	if (!UKRVBlueprintFunctionLibrary::MultiRadialPawnTrace(GetWorld(), OutHits, GetActorLocation(), TauntRange, false))
	{
		return false;
	}
	else
	{
		for (FHitResult OutHit : OutHits)
		{
			AKRVCharacter* CharacterToCheck = Cast<AKRVCharacter>(OutHit.GetActor());
			if (!CharacterToCheck || !IsInChargeRange(CharacterToCheck))
			{
				continue;
			}
			else
			{
				OutEnemies.Emplace(CharacterToCheck);
			}
		}

		if (OutEnemies.Num() > 0)
		{
			return true;
		}
	}
	return false;
}

bool AKRVCharacter::IsAbleToMeleeAttack() const
{
	if (!GetWorld() || !IsAbleToAttack() || !GetCharacterDataAsset())
	{
		return false;
	}
	else if (GetWorldTimerManager().GetTimerRemaining(MeleeAttackRateTimer) > 0.1f)
	{
		return false;
	}
	else if (GetCharacterDataAsset()->bIsFlying)
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool AKRVCharacter::IsInMeleeAttackRange() const
{
	AKRVCharacter* CharacterToMelee = Cast<AKRVCharacter>(GetTargetUnit());
	if (CharacterToMelee)
	{
		bool bIsAliveSuitableTarget = UKRVBlueprintFunctionLibrary::IsSuitableTarget(GetTargetUnit(), this, ETargetType::HostileCharacter) ||
			GetTargetUnit()->GetActionStatus() != EActionStatus::Dead;
		bool bIsNotFlyingUnit = !CharacterToMelee->GetCharacterDataAsset()->bIsFlying;
		bool bIsInRange = GetHorizontalDistanceTo(CharacterToMelee) <= GetMeleeAttackableDistance();
		bool bIsInSight = IsUnitInSight();
		bool bDoesTargetUnitSetEachOther = CharacterToMelee->GetTargetUnit() == Cast<AKRVCombatUnit>(this);
		if (bIsAliveSuitableTarget && bIsNotFlyingUnit && bIsInRange && bIsInSight && bDoesTargetUnitSetEachOther)
		{
			return true;
		}
	}
	return false;

}

bool AKRVCharacter::MeleeAttack()
{
	if (!IsInMeleeAttackRange())
	{
		return false;
	}
	else if (!IsAbleToMeleeAttack())
	{
		return false;
	}
	else
	{
		if (!PlayAttackMontage(GetCurrentWeaponType(), GetModifiedCharacterStat()->MeleeAttackStat.AttackRate))
		{
			KRVLOG(Error, TEXT("Couldn't play montage"));
			return false;
		}
		else
		{
			SetActionStatus(EActionStatus::Attacking);
			SetCombatStatus(ECombatStatus::Melee);
			EnableWalk(false);

			WeaponSwap(ECombatStatus::Melee);
			GetWorldTimerManager().ClearTimer(AttackStatusTimer);
			GetWorldTimerManager().SetTimer(MeleeAttackRateTimer, this,
				&AKRVCharacter::OnAttackCooldownTimeUp, GetModifiedCharacterStat()->MeleeAttackStat.AttackRate, false);
			return true;
		}
	}
	return false;

}

void AKRVCharacter::OnMeleeAttackFire()
{
	AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(this);
	if (!KRVInGameGameMode || !GetTargetUnit())
	{
		KRVCHECK(KRVInGameGameMode);
		KRVCHECK(GetTargetUnit());
		return;
	}
	else if (GetTargetUnit()->GetActionStatus() == EActionStatus::Dead)
	{
		ClearTargetUnit();
		return;
	}
	else
	{
		TArray<FHitResult> HitResults;
		const FName TraceTag("MyTraceTag");
		GetWorld()->DebugDrawTraceTag = TraceTag;
		FCollisionQueryParams CollisionParams;
		CollisionParams.TraceTag = TraceTag;

		GetWorld()->LineTraceMultiByChannel(HitResults, GetMesh()->GetSocketLocation(RightHandSocket),
			GetTargetUnit()->GetActorLocation(), ECollisionChannel::ECC_GameTraceChannel2, CollisionParams);
		for (FHitResult HitResult : HitResults)
		{
			if (Cast<AKRVCombatUnit>(HitResult.GetActor()) == GetTargetUnit())
			{
				KRVInGameGameMode->ApplyDamageAndPlayEffect(HitResult, GetModifiedCharacterStat()->MeleeAttackStat.GetRandDamage(),
					Cast<AKRVCombatUnit>(this), GetModifiedCharacterStat()->MeleeAttackStat.DamageType);
				break;
			}
		}
	}

}
