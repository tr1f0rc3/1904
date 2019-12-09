// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVCombatUnit.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameplayTagContainer.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "WidgetComponent.h"

#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Components/DecalComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "KRV.h"
#include "KRVEnums.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVDataAsset.h"
#include "DataAssets/KRVCombatUnitDataAsset.h"
#include "DataAssets/KRVWeaponDataAsset.h"
#include "DataAssets/KRVBuffDataAsset.h"
#include "DataAssets/KRVShooterComponentDataAsset.h"
#include "DataAssets/KRVAnimInstanceDataAsset.h"
#include "Objects/KRVDamageType.h"

#include "Animations/KRVAnimInstance.h"
#include "SceneComponents/KRVShooterComponent.h"
#include "SceneComponents/PooledComponent.h"
#include "ActorComponents/KRVStatComponent.h"
#include "Actors/KRVBuff.h"
#include "Actors/KRVSkill.h"
#include "Actors/KRVProjectile.h"

#include "Pawns/KRVCharacter.h"
#include "Pawns/KRVTower.h"
#include "Controllers/KRVInGamePlayerController.h"
#include "Controllers/KRVPlayerAIController.h"
#include "Controllers/KRVEnemyAIController.h"
#include "Controllers/KRVAIController.h"
#include "Animations/KRVAnimInstance.h"

#include "KRVGameInstance.h"
#include "GameModes/KRVInGameGameMode.h"
#include "PlayerStates/KRVInGamePlayerState.h"
#include "UIs/KRVCombatUnitUserWidget.h"
#include "UIs/KRVKillRewardUserWidget.h"

AKRVCombatUnit::AKRVCombatUnit()
{
	PrimaryActorTick.bCanEverTick = false;

	// Mesh
	GetMesh()->AddRelativeRotation(FRotator(0.f, -90.f, 0.f));
	GetMesh()->AddRelativeLocation(FVector(0.f, 0.f, -80.f));
	GetMesh()->SetAnimationMode(EAnimationMode::AnimationBlueprint);
	GetMesh()->SetCollisionProfileName("Pawn");
	GetCharacterMovement()->bEnablePhysicsInteraction = false;
	GetCharacterMovement()->MaxAcceleration = 980.f;

	// Spawn
	PooledComponent = CreateDefaultSubobject<UPooledComponent>(TEXT("POOLED_COMPONENT"));
	PooledComponent->OnNearActorActivateDelegate.AddUObject(this, &AKRVCombatUnit::OnSpawned);
	PooledComponent->OnNearActorDeactivateDelegate.AddUObject(this, &AKRVCombatUnit::OnDestroyed);
	PooledComponent->SetupAttachment(RootComponent);

	OnSpawnedParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ON_SPAWNED_PARTICLE_COMPONENT"));
	OnSpawnedParticleComponent->SetupAttachment(GetMesh());
	OnSpawnedParticleComponent->SetAutoActivate(false);
	OnSpawnedAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ON_SPAWNED_AUDIO_COMPONENT"));
	OnSpawnedAudioComponent->SetupAttachment(GetMesh());
	OnSpawnedAudioComponent->SetAutoActivate(false);

	// UI
	CombatUnitWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("COMBAT_UNIT_WIDGET"));
	CombatUnitWidget->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FClassFinder<UKRVCombatUnitUserWidget> COMBAT_UNIT_WIDGET_C(TEXT("/Game/Resources/UI/UI_CombatUnitWidget.UI_CombatUnitWidget_C"));
	if (COMBAT_UNIT_WIDGET_C.Succeeded())
	{
		CombatUnitWidget->AddRelativeLocation(200.f * FVector::UpVector);
		CombatUnitWidget->SetWidgetSpace(EWidgetSpace::Screen);
		CombatUnitWidget->SetWidgetClass(COMBAT_UNIT_WIDGET_C.Class);
		CombatUnitWidget->SetDrawSize(FVector2D(60.f, 80.f));
		CombatUnitWidget->SetHiddenInGame(true);
	}

	KillRewardWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("KILL_REWARD_WIDGET"));
	KillRewardWidget->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FClassFinder<UKRVKillRewardUserWidget> KILL_REWARD_WIDGET_C(TEXT("/Game/Resources/UI/UI_KillRewardWidget.UI_KillRewardWidget_C"));
	if (KILL_REWARD_WIDGET_C.Succeeded())
	{
		KillRewardWidget->AddRelativeLocation(200.f * FVector::UpVector);
		KillRewardWidget->SetWidgetSpace(EWidgetSpace::Screen);
		KillRewardWidget->SetWidgetClass(KILL_REWARD_WIDGET_C.Class);
		KillRewardWidget->SetDrawSize(FVector2D(60.f, 80.f));
	}

	// Control
	SelectionDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("SELECTION_DECAL_COMPONENT"));
	SelectionDecal->SetupAttachment(GetMesh());
	SelectionDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	SelectionDecal->SetRelativeLocation(DecalLocation);
	SelectionDecal->DecalSize.Y = 2.f * GetCapsuleComponent()->GetScaledCapsuleRadius();
	SelectionDecal->DecalSize.Z = 2.f * GetCapsuleComponent()->GetScaledCapsuleRadius();
	SelectionDecal->SetHiddenInGame(true);
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> PLAYER_SELECTION_MI_OBJ
	(TEXT("/Game/Resources/Materials/MAT_D_SelectionCircle_Mint.MAT_D_SelectionCircle_Mint"));
	if (PLAYER_SELECTION_MI_OBJ.Succeeded())
	{
		PlayerSelectionDecalMI = PLAYER_SELECTION_MI_OBJ.Object;
	}
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> ENEMY_SELECTION_MI_OBJ
	(TEXT("/Game/Resources/Materials/MAT_D_SelectionCircle_Red.MAT_D_SelectionCircle_Red"));
	if (ENEMY_SELECTION_MI_OBJ.Succeeded())
	{
		EnemySelectionDecalMI = ENEMY_SELECTION_MI_OBJ.Object;
	}

	SelectionAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("SELECTION_AUDIO_COMPONENT"));
	static ConstructorHelpers::FObjectFinder<USoundCue> SELECTION_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Menu_UI_Beeps/Cues/retro_ui_menu_simple_click_01_Cue.retro_ui_menu_simple_click_01_Cue"));
	if (SELECTION_SOUND_OBJ.Succeeded())
	{
		SelectionAudioComponent->SetupAttachment(GetRootComponent());
		SelectionAudioComponent->SetSound(SELECTION_SOUND_OBJ.Object);
		SelectionAudioComponent->SetAutoActivate(false);
	}

	AIControllerClass = AKRVAIController::StaticClass();
	AutoPossessAI = EAutoPossessAI::Disabled;

	// Stats
	// Buff
	BuffSum = CreateDefaultSubobject<AKRVBuff>(TEXT("BUFF_STAT"));
	BuffSum->SetOwner(this);

	// Status
	StunParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("STUN_PARTICLE_COMPONENT"));
	StunParticleComponent->SetupAttachment(GetMesh(), OverHeadSocket);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> STUN_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Stun/P_Stun_Stars_Base.P_Stun_Stars_Base"));
	if (STUN_PARTICLE_OBJ.Succeeded())
	{
		StunParticleComponent->SetTemplate(STUN_PARTICLE_OBJ.Object);
		StunParticleComponent->SetAutoActivate(false);
	}
	StunAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("STUN_AUDIO_COMPONENT"));
	StunAudioComponent->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FObjectFinder<USoundCue> STUN_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Magic/Cues/retro_magic_spell_cast_sparkle_26_Cue.retro_magic_spell_cast_sparkle_26_Cue"));
	if (STUN_SOUND_OBJ.Succeeded())
	{
		StunAudioComponent->SetSound(STUN_SOUND_OBJ.Object);
		StunAudioComponent->SetAutoActivate(false);
	}

	// Ranged
	FireRadiusDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("RANGED_RANGE_DECAL_COMPONENT"));
	FireRadiusDecal->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> RANGED_RANGE_MI_OBJ
	(TEXT("/Game/Resources/Materials/MAT_D_SelectionCircle_Yellow.MAT_D_SelectionCircle_Yellow"));
	if (RANGED_RANGE_MI_OBJ.Succeeded())
	{
		FireRadiusDecal->SetDecalMaterial(RANGED_RANGE_MI_OBJ.Object);
		FireRadiusDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		FireRadiusDecal->SetRelativeLocation(DecalLocation);
	}
	FireRadiusDecal->SetHiddenInGame(true);

	LeftRangedWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LEFT_RANGED_WEAPON"));
	LeftRangedWeaponMesh->SetupAttachment(GetMesh(), LeftHandSocket);
	RightRangedWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RIGHT_RANGED_WEAPON"));
	RightRangedWeaponMesh->SetupAttachment(GetMesh(), RightHandSocket);

	ShooterComponent = CreateDefaultSubobject<UKRVShooterComponent>(TEXT("SHOOTER_COMPONENT"));
	ShooterComponent->SetupAttachment(GetMesh(), RightHandSocket);

	DeathParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("DEATH_PARTICLE_COMPONENT"));
	DeathParticleComponent->SetupAttachment(GetMesh());
	DeathParticleComponent->SetAutoActivate(false);
	DeathAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("DEATH_AUDIO_COMPONENT"));
	DeathAudioComponent->SetupAttachment(GetRootComponent());
	DeathAudioComponent->SetAutoActivate(false);

	// Reward
	static ConstructorHelpers::FObjectFinder<UParticleSystem> COIN_PARTICLE_OBJ(TEXT
	("/Game/Resources/Particles/P_FloatingCoin.P_FloatingCoin"));
	if (COIN_PARTICLE_OBJ.Succeeded())
	{
		CoinParticle = COIN_PARTICLE_OBJ.Object;
	}

	static ConstructorHelpers::FObjectFinder<UParticleSystem> RUBY_PARTICLE_OBJ(TEXT
	("/Game/Resources/Particles/P_FloatingRuby.P_FloatingRuby"));
	if (RUBY_PARTICLE_OBJ.Succeeded())
	{
		RubyParticle = RUBY_PARTICLE_OBJ.Object;
	}

	// Upgrade
	static ConstructorHelpers::FObjectFinder<UParticleSystem> UPGRADE_PARTICLE_OBJ(TEXT
	("/Game/InfinityBladeEffects/Effects/FX_Skill_RockBurst/P_RBurst_Base_Charge_01.P_RBurst_Base_Charge_01"));
	if (UPGRADE_PARTICLE_OBJ.Succeeded())
	{
		UpgradeParticle = UPGRADE_PARTICLE_OBJ.Object;
	}

}

void AKRVCombatUnit::BeginPlay()
{
	Super::BeginPlay();
	BindToWidgetsOnBeginPlay();
	OnBeginPlayAndSpawned();

}

UKRVDataAsset* AKRVCombatUnit::GetDataAsset_Implementation() const
{
	return HardPtr_DataAsset;

}

void AKRVCombatUnit::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	LoadFromDataAsset_Implementation();

	KRVCHECK(GetKRVAnimInstance());
	if (GetKRVAnimInstance())
	{
		GetKRVAnimInstance()->OnAttackFireNotifyDelegate.AddUObject(this, &AKRVCombatUnit::OnAnimFireNotified);
	}

	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		if (KRVInGameGameMode)
		{
			OnDieDelegate.AddUObject(KRVInGameGameMode, &AKRVInGameGameMode::OnUnitDie);
			OnDieWithRewardDelegate.AddUObject(KRVInGameGameMode, &AKRVInGameGameMode::OnUnitDieWithReward);
			KRVInGameGameMode->OnGameEndDelegate.AddUObject(this, &AKRVCombatUnit::OnGameEnd);
		}
	}
}

void AKRVCombatUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AKRVCombatUnit::PossessedBy(AController* NewController)
{
	if (!NewController)
	{
		KRVCHECK(NewController);
		return;
	}
	Super::PossessedBy(NewController);

}

UKRVCombatUnitDataAsset* AKRVCombatUnit::GetCombatUnitDataAsset() const
{
	return Cast<UKRVCombatUnitDataAsset>(GetDataAsset_Implementation());

}

bool AKRVCombatUnit::LoadFromDataAsset_Implementation()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	HardPtr_DataAsset = Cast<UKRVDataAsset>(Streamable.LoadSynchronous(SoftPtr_DataAsset.ToSoftObjectPath(), true));
	if (!HardPtr_DataAsset)
	{
		KRVCHECK(HardPtr_DataAsset);
		return false;
	}
	else
	{
		LoadBaseStat();
		LoadRangedWeapon();

		OnSpawnedParticlePath = GetCombatUnitDataAsset()->OnSpawnedParticle.ToSoftObjectPath();
		Streamable.RequestAsyncLoad(OnSpawnedParticlePath, FStreamableDelegate::CreateUObject(this,
			&AKRVCombatUnit::OnSpawnedParticleAsyncLoaded));

		OnSpawnedSoundPath = GetCombatUnitDataAsset()->OnSpawnedSound.ToSoftObjectPath();
		Streamable.RequestAsyncLoad(OnSpawnedSoundPath, FStreamableDelegate::CreateUObject(this,
			&AKRVCombatUnit::OnSpawnedSoundAsyncLoaded));

		OnDestroyedParticlePath = GetCombatUnitDataAsset()->OnDestroyedParticle.ToSoftObjectPath();
		Streamable.RequestAsyncLoad(OnDestroyedParticlePath, FStreamableDelegate::CreateUObject(this,
			&AKRVCombatUnit::OnDestroyedParticleAsyncLoaded));

		DeathParticlePath = GetCombatUnitDataAsset()->DeathParticle.ToSoftObjectPath();
		Streamable.RequestAsyncLoad(DeathParticlePath, FStreamableDelegate::CreateUObject(this,
			&AKRVCombatUnit::DeathParticleAsyncLoaded));

		DeathSoundPath = GetCombatUnitDataAsset()->DeathSound.ToSoftObjectPath();
		Streamable.RequestAsyncLoad(DeathSoundPath, FStreamableDelegate::CreateUObject(this,
			&AKRVCombatUnit::DeathSoundAsyncLoaded));

		// Anim
		HardPtr_AnimInstanceDataAsset = Cast<UKRVAnimInstanceDataAsset>
			(Streamable.LoadSynchronous(GetCombatUnitDataAsset()->AnimInstanceDataAsset.ToSoftObjectPath(), true));

		KRVCHECK(HardPtr_AnimInstanceDataAsset);
		if (HardPtr_AnimInstanceDataAsset)
		{
			GetMesh()->SetAnimInstanceClass(HardPtr_AnimInstanceDataAsset->AnimInstanceClass);

			GetHitMontagePath = HardPtr_AnimInstanceDataAsset->GetHitMontage.ToSoftObjectPath();
			Streamable.RequestAsyncLoad(GetHitMontagePath, FStreamableDelegate::CreateUObject(this,
				&AKRVCombatUnit::OnGetHitMontageAsyncLoaded));

			LoadAttackMontages();

			StunMontagePath = HardPtr_AnimInstanceDataAsset->StunMontage.ToSoftObjectPath();
			Streamable.RequestAsyncLoad(StunMontagePath, FStreamableDelegate::CreateUObject(this,
				&AKRVCombatUnit::OnStunMontageAsyncLoaded));

			CastingMontagePath = HardPtr_AnimInstanceDataAsset->CastingMontage.ToSoftObjectPath();
			Streamable.RequestAsyncLoad(CastingMontagePath, FStreamableDelegate::CreateUObject(this,
				&AKRVCombatUnit::OnCastingMontageAsyncLoaded));

			VictoryMontagePath = HardPtr_AnimInstanceDataAsset->VictoryMontage.ToSoftObjectPath();
			Streamable.RequestAsyncLoad(VictoryMontagePath, FStreamableDelegate::CreateUObject(this,
				&AKRVCombatUnit::OnVictoryMontageAsyncLoaded));

			WeaponSwapMontagePath = HardPtr_AnimInstanceDataAsset->WeaponSwapMontage.ToSoftObjectPath();
			Streamable.RequestAsyncLoad(WeaponSwapMontagePath, FStreamableDelegate::CreateUObject(this,
				&AKRVCombatUnit::OnWeaponSwapMontageAsyncLoaded));

			DieMontagePath = HardPtr_AnimInstanceDataAsset->DieMontage.ToSoftObjectPath();
			Streamable.RequestAsyncLoad(DieMontagePath, FStreamableDelegate::CreateUObject(this,
				&AKRVCombatUnit::OnDieMontageAsyncLoaded));
		}

		// Skill
		for (auto SkillClass : GetCombatUnitDataAsset()->SkillClass)
		{
			AcquireSkill(SkillClass);
		}
		return true;
	}

}

bool AKRVCombatUnit::LoadRangedWeapon(int32 UpgradeIndex)
{
	if (!GetCombatUnitDataAsset())
	{
		KRVCHECK(GetCombatUnitDataAsset());
		return false;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	bool bIsLeftRangedWeaponLoaded = false;
	if (GetMesh()->DoesSocketExist(LeftHandSocket))
	{
		// left hand
		if (UpgradeIndex >= GetCombatUnitDataAsset()->LeftRangedWeapon.Num() || UpgradeIndex < 0)
		{
			KRVLOG(Warning, TEXT("Not available index %d"), UpgradeIndex);
		}
		else
		{
			UKRVWeaponDataAsset* LeftRangedWeaponDataAsset = Cast<UKRVWeaponDataAsset>(Streamable.LoadSynchronous
			((GetCombatUnitDataAsset()->LeftRangedWeapon)[UpgradeIndex].ToSoftObjectPath(), true));
			if (LeftRangedWeaponDataAsset)
			{
				LeftRangedWeaponMeshPath = LeftRangedWeaponDataAsset->WeaponMesh.ToSoftObjectPath();
				Streamable.RequestAsyncLoad(LeftRangedWeaponMeshPath, FStreamableDelegate::CreateUObject(this,
					&AKRVCombatUnit::OnLeftRangedWeaponAsyncLoaded));
				RangedWeaponType = LeftRangedWeaponDataAsset->WeaponType;
				bIsLeftRangedWeaponLoaded = true;
			}
		}
	}

	bool bIsRightRangedWeaponLoaded = false;
	bool bIsShooterComponentLoaded = false;
	if (GetMesh()->DoesSocketExist(RightHandSocket))
	{
		// right hand
		if (UpgradeIndex >= GetCombatUnitDataAsset()->RightRangedWeapon.Num() || UpgradeIndex < 0)
		{
			KRVLOG(Warning, TEXT("Not available index %d"), UpgradeIndex);
		}
		else
		{
			UKRVWeaponDataAsset* RightRangedWeaponDataAsset = Cast<UKRVWeaponDataAsset>(Streamable.LoadSynchronous
			((GetCombatUnitDataAsset()->RightRangedWeapon)[UpgradeIndex].ToSoftObjectPath(), true));
			if (RightRangedWeaponDataAsset)
			{
				RightRangedWeaponMeshPath = RightRangedWeaponDataAsset->WeaponMesh.ToSoftObjectPath();
				Streamable.RequestAsyncLoad(RightRangedWeaponMeshPath, FStreamableDelegate::CreateUObject(this,
					&AKRVCombatUnit::OnRightRangedWeaponAsyncLoaded));
				RangedWeaponType = RightRangedWeaponDataAsset->WeaponType;
				bIsRightRangedWeaponLoaded = true;
			}
		}

		// projectile 
		if (ShooterComponent && UpgradeIndex < GetCombatUnitDataAsset()->ShooterComponent.Num() && UpgradeIndex >= 0)
		{
			HardPtr_MyShooterComponentDataAsset =
				Cast<UKRVShooterComponentDataAsset>(Streamable.LoadSynchronous
				((GetCombatUnitDataAsset()->ShooterComponent)[UpgradeIndex].ToSoftObjectPath(), true));
			ShooterComponent->SetDataAssetAndLoad(HardPtr_MyShooterComponentDataAsset);
			ShooterComponent->SetShooter(this);
			ShooterComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocket);
			ShooterComponent->AddRelativeRotation(FRotator(0.f, 0.f, 0.f));
			bIsShooterComponentLoaded = true;
		}
	}
	return bIsLeftRangedWeaponLoaded || bIsRightRangedWeaponLoaded || bIsShooterComponentLoaded;

}

void AKRVCombatUnit::OnLeftRangedWeaponAsyncLoaded()
{
	TSoftObjectPtr<UStaticMesh> AssetPathPtr(LeftRangedWeaponMeshPath);
	if (AssetPathPtr.IsValid())
	{
		LeftRangedWeaponMesh->SetStaticMesh(AssetPathPtr.Get());
		LeftRangedWeaponMesh->SetCollisionProfileName("NoCollision");
		LeftRangedWeaponMesh->AttachToComponent
		(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, LeftHandSocket);
	}

}

void AKRVCombatUnit::OnRightRangedWeaponAsyncLoaded()
{
	TSoftObjectPtr<UStaticMesh> AssetPathPtr(RightRangedWeaponMeshPath);
	if (AssetPathPtr.IsValid())
	{
		RightRangedWeaponMesh->SetStaticMesh(AssetPathPtr.Get());
		RightRangedWeaponMesh->SetCollisionProfileName("NoCollision");
		RightRangedWeaponMesh->AttachToComponent
		(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, RightHandSocket);
	}

}


void AKRVCombatUnit::OnSpawnedParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(OnSpawnedParticlePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		OnSpawnedParticleComponent->SetTemplate(AssetPathPtr.Get());
	}

}

void AKRVCombatUnit::OnSpawnedSoundAsyncLoaded()
{
	TSoftObjectPtr<USoundCue> AssetPathPtr(OnSpawnedSoundPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		OnSpawnedAudioComponent->SetSound(AssetPathPtr.Get());
	}

}

void AKRVCombatUnit::OnDestroyedParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(OnDestroyedParticlePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		OnDestroyedParticle = AssetPathPtr.Get();
	}

}

void AKRVCombatUnit::DeathParticleAsyncLoaded()
{
	TSoftObjectPtr<UParticleSystem> AssetPathPtr(DeathParticlePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		DeathParticleComponent->SetTemplate(AssetPathPtr.Get());
	}

}

void AKRVCombatUnit::DeathSoundAsyncLoaded()
{
	TSoftObjectPtr<USoundCue> AssetPathPtr(DeathSoundPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		DeathAudioComponent->SetSound(AssetPathPtr.Get());
	}

}


void AKRVCombatUnit::LoadAttackMontages()
{
	KRVCHECK(HardPtr_AnimInstanceDataAsset);
	if (HardPtr_AnimInstanceDataAsset)
	{
		AttackMontageLoadIndex = -1;
		AttackMontages.Empty();
		AttackMontages.Init(nullptr, HardPtr_AnimInstanceDataAsset->AttackMontages.Num());
		LoadAttackMontages_Recursive();
	}

}

void AKRVCombatUnit::OnGetHitMontageAsyncLoaded()
{
	TSoftObjectPtr<UAnimMontage> AssetPathPtr(GetHitMontagePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		GetHitMontage = AssetPathPtr.Get();
	}

}
void AKRVCombatUnit::LoadAttackMontages_Recursive()
{
	if (++AttackMontageLoadIndex < HardPtr_AnimInstanceDataAsset->AttackMontages.Num())
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		AttackMontagePath = HardPtr_AnimInstanceDataAsset->AttackMontages[AttackMontageLoadIndex].ToSoftObjectPath();
		Streamable.RequestAsyncLoad(AttackMontagePath, FStreamableDelegate::CreateUObject(this,
			&AKRVCombatUnit::OnAttackMontageAsyncLoaded));
	}
}

void AKRVCombatUnit::OnAttackMontageAsyncLoaded()
{
	TSoftObjectPtr<UAnimMontage> AssetPathPtr(AttackMontagePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		AttackMontages[AttackMontageLoadIndex] = AssetPathPtr.Get();
	}
	LoadAttackMontages_Recursive();

}

void AKRVCombatUnit::OnStunMontageAsyncLoaded()
{
	TSoftObjectPtr<UAnimMontage> AssetPathPtr(StunMontagePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		StunMontage = AssetPathPtr.Get();
	}

}

void AKRVCombatUnit::OnCastingMontageAsyncLoaded()
{
	TSoftObjectPtr<UAnimMontage> AssetPathPtr(CastingMontagePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		CastingMontage = AssetPathPtr.Get();
	}

}

void AKRVCombatUnit::OnVictoryMontageAsyncLoaded()
{
	TSoftObjectPtr<UAnimMontage> AssetPathPtr(VictoryMontagePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		VictoryMontage = AssetPathPtr.Get();
	}

}

void AKRVCombatUnit::OnWeaponSwapMontageAsyncLoaded()
{
	TSoftObjectPtr<UAnimMontage> AssetPathPtr(WeaponSwapMontagePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		WeaponSwapMontage = AssetPathPtr.Get();
	}

}

void AKRVCombatUnit::OnDieMontageAsyncLoaded()
{
	TSoftObjectPtr<UAnimMontage> AssetPathPtr(DieMontagePath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		DieMontage = AssetPathPtr.Get();
	}

}

bool AKRVCombatUnit::LoadBaseStat()
{
	return true;
}


bool AKRVCombatUnit::IsCurrentlySelected() const
{
	return bIsCurrentlySelected;

}

AKRVAIController* AKRVCombatUnit::GetPossessingKRVAIController()
{
	return Cast<AKRVAIController>(UAIBlueprintHelperLibrary::GetAIController(Cast<AActor>(this)));

}

void AKRVCombatUnit::OnSelected(bool bIsSelected)
{
	if (bIsCurrentlySelected != bIsSelected)
	{
		bIsCurrentlySelected = bIsSelected;

		SelectionDecal->SetHiddenInGame(!bIsSelected);
		ShowFireRadiusDecal(bIsSelected);
		SelectionAudioComponent->Play();
		OnSelectedDelegate.Broadcast(bIsSelected, this);
	}

}

void AKRVCombatUnit::BindToWidgetsOnBeginPlay()
{
	UKRVKillRewardUserWidget* CastedKillRewardUserWidget = Cast<UKRVKillRewardUserWidget>(KillRewardWidget->GetUserWidgetObject());
	KRVCHECK(CastedKillRewardUserWidget);
	if (CastedKillRewardUserWidget)
	{
		CastedKillRewardUserWidget->BindToCombatUnit(this);
	}

}

void AKRVCombatUnit::BindToWidgetsBySpawner()
{
	// not implemented;

}

class UKRVAnimInstance* AKRVCombatUnit::GetKRVAnimInstance() const
{
	return Cast<UKRVAnimInstance>(GetMesh()->GetAnimInstance());

}

bool AKRVCombatUnit::PlayAttackMontage(EWeaponType NewWeaponType, float AttackRate/*= 0*/)
{
	if (!GetKRVAnimInstance())
	{
		KRVCHECK(GetKRVAnimInstance());
		return false;
	}
	else if (AttackMontages.Num() > 0)
	{
		int32 PlayIndex = FMath::RandRange(0, AttackMontages.Num() - 1);
		UAnimMontage* MontageToPlay = AttackMontages[PlayIndex];
		KRVCHECK(MontageToPlay);
		if (MontageToPlay)
		{
			float PlayRate = 1.f;
			float MontagePlayTime = MontageToPlay->CalculateSequenceLength();
			if (AttackRate != 0 && MontagePlayTime > AttackRate + 0.1f)
			{
				PlayRate = (MontagePlayTime + 0.1f) / AttackRate;
			}

			if (GetKRVAnimInstance()->Montage_Play(MontageToPlay, PlayRate) > 0.f)
			{
				return true;
			}
		}
	}
	return false;

}

bool AKRVCombatUnit::PlayCastingMontage(EWeaponType NewWeaponType)
{
	if (!GetKRVAnimInstance() || !CastingMontage)
	{
		KRVCHECK(GetKRVAnimInstance());
		KRVCHECK(CastingMontage);
		return false;
	}
	else
	{
		FName SlotName = GetKRVAnimInstance()->GetSlotNameByEnum(EBlendPart::FullBody, NewWeaponType);

		if (GetKRVAnimInstance()->Montage_Play(CastingMontage) > 0.f)
		{
			return true;
		}
	}
	return false;

}

bool AKRVCombatUnit::PlayStunMontage()
{
	if (!GetKRVAnimInstance() || !StunMontage)
	{
		KRVCHECK(GetKRVAnimInstance());
		KRVCHECK(StunMontage);
		return false;
	}
	else if (GetKRVAnimInstance()->Montage_Play(StunMontage) > 0.f)
	{
		return true;
	}
	return false;

}

bool AKRVCombatUnit::PlayVictoryMontage(EWeaponType NewWeaponType)
{
	if (!GetKRVAnimInstance() || !VictoryMontage)
	{
		KRVCHECK(GetKRVAnimInstance());
		KRVCHECK(VictoryMontage);
		return false;
	}
	else
	{
		if (GetKRVAnimInstance()->Montage_Play(VictoryMontage) > 0.f)
		{
			return true;
		}
	}
	return false;

}

bool AKRVCombatUnit::PlayDieMontage(EWeaponType NewWeaponType)
{
	if (!GetKRVAnimInstance() || !DieMontage)
	{
		KRVCHECK(GetKRVAnimInstance());
		KRVCHECK(DieMontage);
		return false;
	}
	else
	{
		FName SlotName = GetKRVAnimInstance()->GetSlotNameByEnum(EBlendPart::FullBody, NewWeaponType);
		float PlayLength = GetKRVAnimInstance()->Montage_Play(DieMontage, 1.f, EMontagePlayReturnType::MontageLength);
		if (PlayLength > 0.f && GetWorld())
		{
			GetWorldTimerManager().SetTimer(MontageDelayTimer, this, &AKRVCombatUnit::FreezeAsDead, PlayLength - 0.01f, false);
			return true;
		}
	}
	return false;

}

void AKRVCombatUnit::FreezeAsDead()
{
	KRVCHECK(DieMontage);
	if (DieMontage)
	{
		float PlayLength = DieMontage->GetPlayLength();
		GetKRVAnimInstance()->Montage_Play(DieMontage, 0.f, EMontagePlayReturnType::MontageLength, PlayLength);
	}

}

bool AKRVCombatUnit::PlayWeaponSwapMontage(EWeaponType NewWeaponType)
{
	if (!GetKRVAnimInstance() || !WeaponSwapMontage)
	{
		KRVCHECK(GetKRVAnimInstance());
		KRVCHECK(WeaponSwapMontage);
		return false;
	}
	else
	{
		if (GetKRVAnimInstance()->Montage_Play(WeaponSwapMontage) > 0.f)
		{
			return true;
		}
	}
	return false;

}

FName AKRVCombatUnit::GetClassFName() const
{
	if (GetCombatUnitDataAsset())
	{
		return GetCombatUnitDataAsset()->ClassFName;
	}
	return FName();

}

EKRVTeam AKRVCombatUnit::GetTeam() const
{
	return TeamID;

}

int32 AKRVCombatUnit::GetUnitInFieldID() const
{
	return UnitInFieldID;

}

bool AKRVCombatUnit::PlayBrainComponent(bool bIsTrue)
{
	AAIController* AIController = UAIBlueprintHelperLibrary::GetAIController(this);
	if (!AIController)
	{
		return false;
	}
	else
	{
		UBrainComponent* BrainComponent = AIController->GetBrainComponent();
		if (!BrainComponent)
		{
			return false;
		}
		else if (bIsTrue)
		{
			BrainComponent->RestartLogic();
		}
		else
		{
			BrainComponent->StopLogic("");
		}
		return true;
	}

}

void AKRVCombatUnit::BeUnPossessedAndDestroyController()
{
	AController* ControllerToBeReplaced = GetController();
	if (ControllerToBeReplaced)
	{
		ControllerToBeReplaced->UnPossess();
		ControllerToBeReplaced->ConditionalBeginDestroy();
	}

}

void AKRVCombatUnit::OnAnimFireNotified()
{
	switch (GetActionStatus())
	{
	case EActionStatus::Casting:
	{
		OnSkillCasted();
		break;
	}
	case EActionStatus::Attacking:
	{
		if (GetCombatStatus() == ECombatStatus::Ranged)
		{
			OnFired();
			break;
		}
	}
	default:
		break;
	}

}


void AKRVCombatUnit::BePossessedByAIController(EKRVTeam Team, bool bDoesPlayNow /*= true*/)
{
	if (GetTeam() == Team)
	{
		if (UAIBlueprintHelperLibrary::GetAIController(this))
		{
			// do nothing;
		}
		else if (MainAIController.IsValid())
		{
			MainAIController.Get()->Possess(this);
		}
	}
	else
	{
		SetTeam(Team);
		BeUnPossessedAndDestroyController();
		SpawnDefaultController();
		MainAIController = Cast<AKRVAIController>(UAIBlueprintHelperLibrary::GetAIController(this));
	}

	PlayBrainComponent(bDoesPlayNow);

}

bool AKRVCombatUnit::IsFriendly(AKRVCombatUnit* _Target) const
{
	return GetTeam() == _Target->GetTeam();

}

bool AKRVCombatUnit::IsHostile(AKRVCombatUnit* _Target) const
{
	return GetTeam() != _Target->GetTeam();

}


void AKRVCombatUnit::SetTeam(EKRVTeam NewTeam)
{
	if (TeamID != NewTeam)
	{
		TeamID = NewTeam;
		switch (NewTeam)
		{
		case EKRVTeam::Player:
		{
			SelectionDecal->SetDecalMaterial(PlayerSelectionDecalMI);
			break;
		}
		case EKRVTeam::Enemy:
		{
			SelectionDecal->SetDecalMaterial(EnemySelectionDecalMI);
			break;
		}
		default:
			break;
		}
	}

}

void AKRVCombatUnit::OnBeginPlayAndSpawned()
{
	ResetStatus();
	GetMesh()->SetSimulatePhysics(false);
	GetMesh()->SetEnableGravity(true);
	PlayBrainComponent(true);

	AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(this);
	KRVCHECK(KRVInGameGameMode);
	if (KRVInGameGameMode)
	{
		WaveAtSpawned = KRVInGameGameMode->GetCurrentWave();
		UnitInFieldID = KRVInGameGameMode->IssueUnitInFieldID();
	}

	if (CombatUnitWidget && !bIsGlobalCaster)
	{
		CombatUnitWidget->SetHiddenInGame(false);
	}

	SpawnedLocation = GetActorLocation();

}

void AKRVCombatUnit::OnSpawned()
{
	OnBeginPlayAndSpawned();
	KRVCHECK(GetKRVAnimInstance());
	if (GetKRVAnimInstance())
	{
		PlayVictoryMontage(GetCurrentWeaponType());
	}
	OnSpawnedParticleComponent->ActivateSystem();
	OnSpawnedAudioComponent->Play();

}

void AKRVCombatUnit::OnDestroyed()
{
	if (GetActionStatus() != EActionStatus::Dead)
	{
		PlayBrainComponent(false);
	}

	if (CombatUnitWidget)
	{
		CombatUnitWidget->SetHiddenInGame(true);
	}

	KRVCHECK(GetWorld());
	if (GetWorld() && OnDestroyedParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), OnDestroyedParticle,
			GetActorTransform(), true);
	}

}

void AKRVCombatUnit::SetLocationToSpawnedLocation()
{
	SetActorLocation(SpawnedLocation);
	GetMovementComponent()->Velocity = FVector::ZeroVector;

}

int32 AKRVCombatUnit::GetWaveAtSpawned() const
{
	return WaveAtSpawned;

}

void AKRVCombatUnit::OnGameEnd(EKRVTeam Winner)
{
	if (GetActionStatus() != EActionStatus::Dead)
	{
		StopAction();

		if (GetTeam() == Winner)
		{
			PlayVictoryMontage(GetCurrentWeaponType());
		}
		else
		{
			PlayStunMontage();
		}
	}

}

void AKRVCombatUnit::UpdateStat()
{
	if (!GetBaseStat() || !GetModifiedStat())
	{
		KRVCHECK(GetBaseStat());
		KRVCHECK(GetModifiedStat());
		return;
	}
	GetModifiedStat()->AssignmentOperator(GetBaseStat());
	*GetModifiedStat() += *UpdateBuffStatSum();
	*GetModifiedStat() *= *BuffSum;
	OnStatUpdatedDelegate.Broadcast(this);

}


UKRVStatComponent* AKRVCombatUnit::GetBaseStat() const
{
	return nullptr;

}

UKRVStatComponent* AKRVCombatUnit::GetModifiedStat() const
{
	return nullptr;

}

void AKRVCombatUnit::ShowFireRadiusDecal(bool bDoesShow)
{
	if (!GetBaseStat() || GetBaseStat()->RangedMediumType == ERangedMediumType::NoWeapon ||
		GetActionStatus() == EActionStatus::Dead)
	{
		return;
	}
	else if (FireRadiusDecal)
	{
		FireRadiusDecal->SetHiddenInGame(!bDoesShow);
	}

}

UKRVStatComponent* AKRVCombatUnit::VariateBaseStat(TSubclassOf<AKRVBuff> BuffClass)
{
	if (!GetBaseStat())
	{
		KRVCHECK(GetBaseStat());
		return nullptr;
	}
	Ptr_TemporaryBuff = NewObject<AKRVBuff>(this, BuffClass);
	Ptr_TemporaryBuff->SetOwner(this);
	*GetBaseStat() += *Ptr_TemporaryBuff;
	Ptr_TemporaryBuff = nullptr;
	return GetBaseStat();

}

bool AKRVCombatUnit::TakeBuffWithStatUpdate(AKRVBuff* NewBuff)
{
	bool bIsBuffTaken = TakeBuff(NewBuff);
	if (bIsBuffTaken)
	{
		NewBuff->OnThisBuffRemovedThisPtr.BindUObject(this, &AKRVCombatUnit::RemoveBuff);
		UpdateStat();
	}
	return bIsBuffTaken;

}

bool AKRVCombatUnit::TakeBuffClassWithStatUpdate(TSubclassOf<AKRVBuff> NewBuffClass, AKRVCombatUnit* Caster)
{
	if (!NewBuffClass)
	{
		return false;
	}
	Ptr_TemporaryBuff = NewObject<AKRVBuff>(this, NewBuffClass);
	Ptr_TemporaryBuff->SetOwner(this);
	if (!Ptr_TemporaryBuff || !Ptr_TemporaryBuff->GiveThis(this, Caster))
	{
		KRVCHECK(Ptr_TemporaryBuff);
		return false;
	}
	else
	{
		Ptr_TemporaryBuff = nullptr;
		return true;
	}


}

bool AKRVCombatUnit::RemoveBuffWithStatUpdate(AKRVBuff* NewBuff)
{
	bool bIsBuffRemoved = RemoveBuff(NewBuff);
	if (bIsBuffRemoved)
	{
		UpdateStat();
	}
	return bIsBuffRemoved;

}

void AKRVCombatUnit::RemoveAllBuffsWithStatUpdate(ETargetType NewTargetType)
{
	RemoveAllBuffs(NewTargetType);
	UpdateStat();

}

AKRVBuff* AKRVCombatUnit::UpdateBuffStatSum()
{
	AKRVBuff* DummyBuff = NewObject<AKRVBuff>(this);
	for (auto Buff : Buffs)
	{
		*DummyBuff += *(Buff);
	}
	for (auto Debuff : Debuffs)
	{
		*DummyBuff += *(Debuff);
	}
	if (BuffSum)
	{
		BuffSum->ConditionalBeginDestroy();
	}
	BuffSum = DummyBuff;
	BuffSum->SetOwner(this);
	return BuffSum;

}

bool AKRVCombatUnit::TakeBuff(AKRVBuff* NewBuff)
{
	if (!NewBuff || GetActionStatus() == EActionStatus::Dead)
	{
		KRVCHECK(NewBuff);
		KRVCHECK(GetActionStatus() != EActionStatus::Dead);
		return false;
	}
	else if (!NewBuff->GetDataAsset() || !(NewBuff->GetBuffCaster()))
	{
		KRVCHECK(NewBuff->GetDataAsset());
		KRVCHECK(NewBuff->GetBuffCaster());
	}
	else if (!UKRVBlueprintFunctionLibrary::IsSuitableTarget
	(NewBuff->BuffOwner, NewBuff->GetBuffCaster(), NewBuff->TargetType))
	{
		return false;
	}
	else
	{
		FGameplayTag NameTag = UKRVBlueprintFunctionLibrary::GetGameplayTagByName(GetDataAsset()->TagContainer, "Name.Buff");
		if (NewBuff->TargetType == ETargetType::FriendlyCharacter || NewBuff->TargetType == ETargetType::FriendlyTower)
		{
			for (auto Buff : Buffs)
			{
				KRVCHECK(Buff->GetDataAsset());
				if (Buff->GetDataAsset() &&
					Buff->GetDataAsset()->TagContainer.HasTag(NameTag))
				{
					if (NewBuff->SpellLevel < Buff->SpellLevel)
					{
						KRVLOG(Log, TEXT("Lower level buff cannot overide higher"));
						return false;
					}
					else
					{
						KRVLOG(Log, TEXT("Buff overrided"));
						RemoveBuff(Buff);
						break;
					}
				}
			}
			Buffs.Emplace(NewBuff);
		}
		else if (NewBuff->TargetType == ETargetType::HostileCharacter || NewBuff->TargetType == ETargetType::HostileTower)
		{
			for (auto Buff : Debuffs)
			{
				KRVCHECK(Buff->GetDataAsset_Implementation());
				if (Buff->GetDataAsset_Implementation() &&
					Buff->GetDataAsset_Implementation()->TagContainer.HasTag(NameTag))
				{
					if (NewBuff->SpellLevel < Buff->SpellLevel)
					{
						KRVLOG(Log, TEXT("Lower level buff cannot overide higher"));
						return false;
					}
					else
					{
						RemoveBuff(Buff);
						KRVLOG(Log, TEXT("Buff overrided"));
						break;
					}
				}
			}
			Debuffs.Emplace(NewBuff);
		}
		return true;
	}
	return false;

}

bool AKRVCombatUnit::RemoveBuff(AKRVBuff* NewBuff)
{
	KRVCHECK(NewBuff);
	if (NewBuff)
	{
		if (NewBuff->TargetType == ETargetType::FriendlyCharacter || NewBuff->TargetType == ETargetType::FriendlyTower)
		{
			int32 index = Buffs.Find(NewBuff);
			if (index == INDEX_NONE)
			{
				KRVLOG(Log, TEXT("Buff %s is not found"), *(NewBuff->GetDataAsset_Implementation()->ClassFName.ToString()));
				return false;
			}
			Buffs[index]->RemoveThis();
			Buffs.RemoveAtSwap(index);
		}
		else if (NewBuff->TargetType == ETargetType::HostileCharacter || NewBuff->TargetType == ETargetType::HostileTower)
		{
			int32 index = Debuffs.Find(NewBuff);
			if (index == INDEX_NONE)
			{
				KRVLOG(Log, TEXT("Debuff %s is not found"), *NewBuff->GetDataAsset_Implementation()->ClassFName.ToString());
				return false;
			}
			Debuffs[index]->RemoveThis();
			Debuffs.RemoveAtSwap(index);
		}
	}
	return true;

}

void AKRVCombatUnit::RemoveAllBuffs(ETargetType NewTargetType)
{
	if (NewTargetType == ETargetType::FriendlyCharacter || NewTargetType == ETargetType::FriendlyTower || NewTargetType == ETargetType::AnyUnit)
	{
		for (auto Buff : Buffs)
		{
			Buff->RemoveThis();
		}
		Buffs.Empty();
	}
	if (NewTargetType == ETargetType::HostileCharacter || NewTargetType == ETargetType::HostileTower || NewTargetType == ETargetType::AnyUnit)
	{
		for (auto Debuff : Debuffs)
		{
			Debuff->RemoveThis();
		}
		Debuffs.Empty();
	}

}

void AKRVCombatUnit::ResetStatus()
{
	SetActionStatus(EActionStatus::Idle, true);
	SetCombatStatus(ECombatStatus::Idle);
	UpdateStat();

}

void AKRVCombatUnit::ClearAllActiveActionTimer()
{
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(SkillCastingTimer);
		GetWorldTimerManager().ClearTimer(RangedAttackRateTimer);
	}

}

void AKRVCombatUnit::ClearAllPassiveActionTimer()
{
	if (GetWorld())
	{
		GetWorldTimerManager().ClearTimer(StunTimer);
	}

}

ECombatStatus AKRVCombatUnit::GetCombatStatus() const
{
	return CurrentCombatStatus;

}

EActionStatus AKRVCombatUnit::GetActionStatus() const
{
	return CurrentActionStatus;

}

bool AKRVCombatUnit::SetCombatStatus(ECombatStatus NewStatus)
{
	if (GetActionStatus() == EActionStatus::Dead)
	{
		return false;
	}
	else if (GetCombatStatus() != NewStatus)
	{
		CurrentCombatStatus = NewStatus;
		OnCombatStatusChangedDelegate.Broadcast(NewStatus, this);
	}
	return true;

}

bool AKRVCombatUnit::SetActionStatus(EActionStatus NewStatus, bool bDoesReset /*= false*/)
{
	if (!bDoesReset && GetActionStatus() == EActionStatus::Dead)
	{
		return false;
	}
	else if (GetActionStatus() != NewStatus)
	{
		LastActionStatus = CurrentActionStatus;
		CurrentActionStatus = NewStatus;
		OnActionStatusChangedDelegate.Broadcast(NewStatus, this);
	}
	return true;

}

bool AKRVCombatUnit::StopAction()
{
	if (GetKRVAnimInstance())
	{
		GetKRVAnimInstance()->StopAllMontages(0.5f);
	}
	CancleSkillCasting();
	return true;

}

void AKRVCombatUnit::CancleSkillCasting()
{
	if (GetActionStatus() == EActionStatus::Casting && GetWorld())
	{
		GetWorldTimerManager().ClearTimer(SkillCastingTimer);
		SetActionStatus(EActionStatus::Idle);

		if (!Skills.IsValidIndex(ReservedSkillIndexToCast))
		{
			KRVCHECK(Skills.IsValidIndex(ReservedSkillIndexToCast));
		}
		else
		{
			OnSkillCastCancleDelegate.Broadcast(Skills[ReservedSkillIndexToCast], this);
		}
	}

}


int32 AKRVCombatUnit::GetSkillIndexByTag(const FGameplayTag& SkillTag) const
{
	const int32* SkillIndexPtr = SkillMap.Find(SkillTag);
	if (!SkillIndexPtr)
	{
		KRVCHECK(SkillIndexPtr);
		return -1;
	}
	else
	{
		return *SkillIndexPtr;
	}

}

float AKRVCombatUnit::GetStunRemainingTime() const
{
	return GetWorldTimerManager().GetTimerRemaining(StunTimer);

}

bool AKRVCombatUnit::MakeStunned(float NewDuration)
{
	if (!GetWorld() || GetActionStatus() == EActionStatus::Dead || NewDuration <= GetStunRemainingTime())
	{
		return false;
	}
	else
	{
		SetActionStatus(EActionStatus::Stunned);
		StopAction();
		PlayBrainComponent(false);

		KRVCHECK(GetKRVAnimInstance());
		if (GetKRVAnimInstance())
		{
			if (!PlayStunMontage())
			{
				KRVLOG(Error, TEXT("Couldn't play montage"));
			}
		}

		StunParticleComponent->ActivateSystem();
		StunAudioComponent->Play();

		if (NewDuration > 0.f)
		{
			GetWorldTimerManager().SetTimer(StunTimer, this, &AKRVCombatUnit::OnStunTimeUp,
				NewDuration, false);
		}
		else
		{
			OnStunTimeUp();
		}
		return true;
	}

}

void AKRVCombatUnit::SetGlobalCaster(bool bIsTrue)
{
	bIsGlobalCaster = bIsTrue;
	bCanBeDamaged = !bIsTrue;
	OnBeginPlayAndSpawned();
	PlayBrainComponent(!bIsTrue);
	SetActorHiddenInGame(bIsTrue);
	SetActorEnableCollision(!bIsTrue);
	GetMesh()->SetEnableGravity(!bIsTrue);

}

void AKRVCombatUnit::OnStunTimeUp()
{
	if (GetActionStatus() != EActionStatus::Dead)
	{
		StopAction();
		SetActionStatus(EActionStatus::Idle);
		PlayBrainComponent(true);
	}
	StunParticleComponent->DeactivateSystem();

}

AKRVCombatUnit* AKRVCombatUnit::GetTargetUnit() const
{
	if (CurrentTargetUnit.IsValid())
	{
		return CurrentTargetUnit.Get();
	}
	return nullptr;

}

FVector AKRVCombatUnit::GetTargetPoint() const
{
	return CurrentTargetPoint;

}

bool AKRVCombatUnit::SetTargetUnit(AKRVCombatUnit* NewTarget)
{
	if (!NewTarget)
	{
		return true;
	}
	else if (NewTarget->GetActionStatus() == EActionStatus::Dead ||
		GetActionStatus() == EActionStatus::Dead)
	{
		return false;
	}
	else
	{
		if (CurrentTargetUnit != NewTarget)
		{
			CurrentTargetUnit = NewTarget;
			ShooterComponent->SetTargetUnit(NewTarget);
			OnTargetUnitChangedDelegate.Broadcast(NewTarget, this);

			StopAction();

			if (GetCombatStatus() != ECombatStatus::ForcedMoving || GetCombatStatus() != ECombatStatus::FollowingPath)
			{
				SetCombatStatus(ECombatStatus::Idle);
			}
		}
		return true;
	}
	return false;

}

bool AKRVCombatUnit::SetTargetPoint(const FVector& NewTargetLocation)
{
	if (!GetWorld())
	{
		KRVCHECK(GetWorld());
		return false;
	}
	CurrentTargetPoint = NewTargetLocation;
	OnTargetPointChangedDelegate.Broadcast(NewTargetLocation, this);
	return true;

}

void AKRVCombatUnit::ClearTargetUnit()
{
	SetTargetUnit(nullptr);

}

bool AKRVCombatUnit::IsUnitInSight(const AKRVCombatUnit* UnitToCheck /*= nullptr*/) const
{
	if (!UnitToCheck)
	{
		UnitToCheck = GetTargetUnit();
	}

	if (!UnitToCheck)
	{
		KRVCHECK(UnitToCheck);
		return false;
	}
	else if (UnitToCheck->GetActionStatus() == EActionStatus::Dead ||
		!CanSeeTheUnitWithTurn(UnitToCheck, false))
	{
		return false;
	}
	else
	{
		float DotProductResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal2D(),
			(UnitToCheck->GetActorLocation() - GetActorLocation()).GetSafeNormal2D());

		if (DotProductResult > FMath::Cos(HalfSightAngle))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;

}

bool AKRVCombatUnit::IsTargetPointInCone() const
{
	float DotProductResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal2D(),
		(GetTargetPoint() - GetActorLocation()).GetSafeNormal2D());
	if (DotProductResult >= FMath::Cos(HalfSightAngle))
	{
		return true;
	}
	else
	{
		return false;
	}
	return false;

}

bool AKRVCombatUnit::CanSeeTheUnitWithTurn(const AKRVCombatUnit* UnitToSee, bool bDoesDrawDebugLine /*= false*/) const
{
	if (!UnitToSee || !GetWorld())
	{
		KRVCHECK(UnitToSee);
		KRVCHECK(GetWorld());
	}
	else
	{
		FCollisionQueryParams CollisionParams;
		if (bDoesDrawDebugLine)
		{
			const FName TraceTag("MyTraceTag");
			GetWorld()->DebugDrawTraceTag = TraceTag;
			CollisionParams.TraceTag = TraceTag;
		}
		FHitResult OutHit;
		GetWorld()->UWorld::LineTraceSingleByChannel(OutHit, GetActorLocation()/*->GetSocketLocation(OverHeadSocket)*/,
			UnitToSee->GetActorLocation()/*->GetSocketLocation(OverHeadSocket)*/, ECC_Visibility, CollisionParams);
		return !bool(OutHit.bBlockingHit);
	}
	return false;

}

ECombatStatus AKRVCombatUnit::FindNearestEnemyRange(class AKRVCharacter*& OutCharacter, ECombatStatus StartRange /*= ECombatStatus::Idle*/)
{
	if (!GetBaseStat() || !GetModifiedStat())
	{
		KRVCHECK(GetBaseStat());
		KRVCHECK(GetModifiedStat());
	}
	else
	{
		TArray<AKRVCharacter*> TargetsToCheck;
		ECombatStatus ReturnValue = ECombatStatus::Idle;

		if (StartRange == ECombatStatus::Ranged || StartRange == ECombatStatus::Melee)
		{
			if (GetBaseStat()->RangedMediumType != ERangedMediumType::NoWeapon &&
				FindVisibleCharactersWithTurn(TargetsToCheck, GetModifiedStat()->RangedAttackStat.AttackRange))
			{
				ReturnValue = ECombatStatus::Ranged;
			}
			else
			{
				StartRange = ECombatStatus::OnGuard;
			}
		}

		if (StartRange == ECombatStatus::OnGuard)
		{
			if (FindVisibleCharactersWithTurn(TargetsToCheck, GetMaxEngageRange()))
			{
				ReturnValue = ECombatStatus::OnGuard;
			}
		}

		if (TargetsToCheck.Num() > 0)
		{
			KRVCHECK(ReturnValue != ECombatStatus::Idle);
			OutCharacter = FindTheNearestFromFowardVector(TargetsToCheck);
		}
		OnNearestEnemyRangeChangedDelegate.Broadcast(ReturnValue, this);
		return ReturnValue;
	}
	return ECombatStatus::NONE;

}

AKRVCharacter* AKRVCombatUnit::FindTheNearestFromFowardVector(TArray<AKRVCharacter*>& TargetsToCheck)
{
	AKRVCharacter* NearestUnit = nullptr;
	if (TargetsToCheck.Num() > 0)
	{
		float HighestCosValue = -2.f;
		for (AKRVCharacter* TargetToCheck : TargetsToCheck)
		{
			float DotProductResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal2D(),
				(TargetToCheck->GetActorLocation() - GetActorLocation()).GetSafeNormal2D());
			if (DotProductResult > HighestCosValue)
			{
				NearestUnit = TargetToCheck;
				HighestCosValue = DotProductResult;
			}
		}
	}
	return NearestUnit;

}

AKRVCharacter* AKRVCombatUnit::FindTheNearestCharacter(TArray<AKRVCharacter*>& TargetsToCheck)
{
	AKRVCharacter* NearestUnit = nullptr;
	if (TargetsToCheck.Num() > 0)
	{
		float LowestDistValue = 10000.f;
		for (AKRVCharacter* TargetToCheck : TargetsToCheck)
		{
			float DistValue = GetHorizontalDistanceTo(TargetToCheck);
			if (DistValue < LowestDistValue)
			{
				NearestUnit = TargetToCheck;
				LowestDistValue = DistValue;
			}
		}
	}
	return NearestUnit;

}

bool AKRVCombatUnit::FindVisibleCharactersWithTurn(TArray<AKRVCharacter*>& OutEnemies, float SightRadius, bool bIsFriendly /*= false*/) const
{
	OutEnemies.Empty();
	TArray<FHitResult> OutHits;
	if (UKRVBlueprintFunctionLibrary::MultiRadialPawnTrace(GetWorld(), OutHits, GetActorLocation(), SightRadius, false))
	{
		for (FHitResult OutHit : OutHits)
		{
			AKRVCharacter* CharacterToCheck = Cast<AKRVCharacter>(OutHit.GetActor());
			if (!CharacterToCheck || !CanSeeTheUnitWithTurn(CharacterToCheck) ||
				(bIsFriendly && !IsFriendly(CharacterToCheck)) ||
				(!bIsFriendly && !IsHostile(CharacterToCheck)))
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

bool AKRVCombatUnit::IsAbleToAttack() const
{
	if (!IsAbleToMove() || !(GetActionStatus() == EActionStatus::Idle || GetActionStatus() == EActionStatus::Moving))
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool AKRVCombatUnit::IsAbleToMove() const
{
	if (GetActionStatus() == EActionStatus::Dead ||
		GetActionStatus() == EActionStatus::Stunned)
	{
		return false;
	}
	else
	{
		return true;
	}

}

void AKRVCombatUnit::ReserveAction(const FKRVReservedAction& NewAction)
{
	ReservedActionsToStack.Emplace(NewAction);

}

bool AKRVCombatUnit::DoReservedAction()
{
	if (ReservedActionsToPop.Num() == 0)
	{
		if (ReservedActionsToStack.Num() == 0)
		{
			return false;
		}
		else
		{
			ReservedActionsToPop.Reserve(ReservedActionsToStack.Num());
			for (int32 PopIndex = ReservedActionsToStack.Num() - 1; PopIndex >= 0; --PopIndex)
			{
				ReservedActionsToPop.Emplace(ReservedActionsToStack.Pop(false));
			}
			ReservedActionsToStack.Shrink();
		}
	}

	KRVCHECK(ReservedActionsToPop.Num() > 0);
	NextActionToDo = ReservedActionsToPop.Pop(true);

	if (NextActionToDo.ActionStatus == EActionStatus::NONE)
	{
		KRVCHECK(NextActionToDo.ActionStatus != EActionStatus::NONE);
		return false;
	}
	else
	{
		if (NextActionToDo.TargetUnit.IsValid())
		{
			SetTargetUnit(NextActionToDo.TargetUnit.Get());
		}
		if (NextActionToDo.TargetPoint != FVector::ZeroVector)
		{
			SetTargetPoint(NextActionToDo.TargetPoint);
		}

		switch (NextActionToDo.ActionStatus)
		{
		case EActionStatus::Idle:
		{
			StopAction();
			break;
		}
		case EActionStatus::Casting:
		{
			CastSkill(NextActionToDo.SkillTagToCast);
			break;
		}
		case EActionStatus::Attacking:
		{
			if (NextActionToDo.CombatStatus == ECombatStatus::Ranged)
			{
				Fire();
			}
			break;
		}
		case EActionStatus::Dead:
		{
			Die();
			break;
		}
		default:
			break;
		}
		return true;
	}

}

EWeaponType AKRVCombatUnit::GetCurrentWeaponType() const
{
	return RangedWeaponType;

}

bool AKRVCombatUnit::IsAbleToFire() const
{
	if (!GetWorld() || !IsAbleToAttack() || GetBaseStat()->RangedMediumType == ERangedMediumType::NoWeapon ||
		!ShooterComponent)
	{
		KRVCHECK(GetBaseStat()->RangedMediumType == ERangedMediumType::NoWeapon ||
			GetBaseStat()->RangedMediumType != ERangedMediumType::NoWeapon && ShooterComponent);
		return false;
	}
	else if (GetWorldTimerManager().GetTimerRemaining(RangedAttackRateTimer) > 0.1f)
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool AKRVCombatUnit::IsInFireRange(AKRVCombatUnit* UnitToShoot /*= nullptr*/) const
{
	if (!UnitToShoot)
	{
		UnitToShoot = GetTargetUnit();
	}

	if (!UnitToShoot)
	{
		KRVCHECK(UnitToShoot);
	}
	else if (!Cast<AKRVCharacter>(UnitToShoot) || UnitToShoot->GetActionStatus() == EActionStatus::Dead)
	{
		return false;
	}
	else if (GetHorizontalDistanceTo(UnitToShoot) > GetModifiedStat()->RangedAttackStat.AttackRange ||
		!IsUnitInSight(UnitToShoot))
	{
		return false;
	}
	else
	{
		return true;
	}
	return false;

}

bool AKRVCombatUnit::Fire()
{
	if (!IsInFireRange())
	{
		ClearTargetUnit();
	}
	else if (!IsAbleToFire())
	{
		// not implemented
	}
	else if (!PlayAttackMontage(GetCurrentWeaponType(), GetModifiedStat()->RangedAttackStat.AttackRate))
	{
		KRVLOG(Error, TEXT("Couldn't play montage"));
	}
	else
	{
		SetActionStatus(EActionStatus::Attacking);
		SetCombatStatus(ECombatStatus::Ranged);
		GetWorldTimerManager().ClearTimer(AttackStatusTimer);
		GetWorldTimerManager().SetTimer(RangedAttackRateTimer, this,
			&AKRVCombatUnit::OnAttackCooldownTimeUp, GetModifiedStat()->RangedAttackStat.AttackRate, false);
		return true;
	}
	return false;

}


void AKRVCombatUnit::OnFired()
{
	if (!GetTargetUnit())
	{
		KRVCHECK(GetTargetUnit());
	}
	else if (GetTargetUnit()->GetActionStatus() == EActionStatus::Dead)
	{
		ClearTargetUnit();
	}
	else
	{
		AKRVProjectile* FiredProjectile = ShooterComponent->Fire(GetModifiedStat()->RangedAttackStat, GetModifiedStat()->SplashAttackStat);
		OnFiringDelegate.Broadcast(FiredProjectile, this);
		OnFiringDynamicDelegate.Broadcast(FiredProjectile, this);
	}

}

void AKRVCombatUnit::OnAttackCooldownTimeUp()
{
	if (IsAbleToMove())
	{
		SetActionStatus(EActionStatus::Idle);
		DoReservedAction();
	}
	GetWorldTimerManager().SetTimer(AttackStatusTimer, this, &AKRVCombatUnit::OnAttackStatusTimeUp, AttackStatusDuration);

}

void AKRVCombatUnit::OnAttackStatusTimeUp()
{
	if (GetCombatStatus() == ECombatStatus::Ranged)
	{
		if (GetActionStatus() == EActionStatus::Stunned || GetActionStatus() == EActionStatus::Casting)
		{
			GetWorldTimerManager().SetTimer(AttackStatusTimer, this, &AKRVCombatUnit::OnAttackStatusTimeUp, AttackStatusDuration);
		}
		else
		{
			SetCombatStatus(ECombatStatus::Idle);
		}
	}

}

float AKRVCombatUnit::GetAttackRemainingTime()
{
	if (!GetWorld())
	{
		return -1.f;
	}
	else
	{
		float RemainingRangedAttackTime = GetWorldTimerManager().GetTimerRemaining(RangedAttackRateTimer);
		float RemainingCastingTime = GetWorldTimerManager().GetTimerRemaining(SkillCastingTimer);
		return FMath::Max3(RemainingRangedAttackTime, RemainingCastingTime, -1.f);
	}

}

bool AKRVCombatUnit::HasActiveSkill() const
{
	for (AKRVSkill* SkillToCheck : Skills)
	{
		KRVCHECK(SkillToCheck);
		if (SkillToCheck && SkillToCheck->GetTargetingType() != ETargetingType::Passive)
		{
			return true;
		}
	}
	return false;

}

bool AKRVCombatUnit::IsSkillReady(const FGameplayTag& SkillTagToCheck)
{
	int32 SkillIndexToCheck = GetSkillIndexByTag(SkillTagToCheck);
	if (!Skills.IsValidIndex(SkillIndexToCheck))
	{
		KRVCHECK(Skills.IsValidIndex(SkillIndexToCheck));
	}
	else
	{
		AKRVSkill* SkillToCast = Skills[SkillIndexToCheck];
		if (!SkillToCast)
		{
			KRVCHECK(SkillToCast);
		}
		else if (SkillToCast->IsAbleToCastSkill())
		{
			return true;

		}
	}
	return false;

}

bool AKRVCombatUnit::IsAbleToCastSomeSkill()
{
	for (const auto& SkillPair : SkillMap)
	{
		if (IsSkillReady(SkillPair.Key))
		{
			return true;
		}
	}
	return false;

}


bool AKRVCombatUnit::IsInSkillCastRange(const FGameplayTag& SkillTagToCast, class AKRVCombatUnit* NewTargetUnit /*= nullptr*/, const FVector& NewTargetPoint /*= FVector::ZeroVector*/) const
{
	int32 SkillIndexToCheck = GetSkillIndexByTag(SkillTagToCast);
	if (!Skills.IsValidIndex(SkillIndexToCheck))
	{
		KRVCHECK(Skills.IsValidIndex(SkillIndexToCheck));
	}
	else
	{
		AKRVSkill* SkillToCast = Skills[SkillIndexToCheck];
		if (!SkillToCast)
		{
			KRVCHECK(SkillToCast);
		}
		else if (bIsGlobalCaster)
		{
			return true;
		}
		else
		{
			switch (SkillToCast->GetTargetingType())
			{
			case ETargetingType::Self:
			{
				return true;
			}
			case ETargetingType::Target:
			{
				if (!NewTargetUnit)
				{
					NewTargetUnit = GetTargetUnit();
				}
				return SkillToCast->IsInSkillRange_Target(NewTargetUnit, this);
			}
			case ETargetingType::Location:
			{
				if (NewTargetPoint == FVector::ZeroVector)
				{
					return SkillToCast->IsInSkillRange_Location(GetTargetPoint(), this);
				}
				else
				{
					return SkillToCast->IsInSkillRange_Location(NewTargetPoint, this);
				}
			}
			default:
				break;
			}
		}
	}
	return false;

}

AKRVCharacter* AKRVCombatUnit::FindTheNearestFromFowardVectorInSkillCastRange(const FGameplayTag& SkillTagToCast, TArray<AKRVCharacter*>& TargetsToCheck)
{
	AKRVSkill* SkillToCheck = GetSkillByTag(SkillTagToCast);
	if (!SkillToCheck || TargetsToCheck.Num() == 0)
	{
		KRVCHECK(SkillToCheck);
		KRVCHECK(TargetsToCheck.Num() > 0);
	}
	else
	{
		AKRVCharacter* NearestUnit = nullptr;
		if (TargetsToCheck.Num() > 0)
		{
			float HighestCosValue = -2.f;
			for (AKRVCharacter* TargetToCheck : TargetsToCheck)
			{
				bool bIsInCastRange = false;
				switch (SkillToCheck->GetTargetingType())
				{
				case ETargetingType::Target:
				{
					bIsInCastRange = SkillToCheck->IsInSkillRange_Target(Cast<AKRVCombatUnit>(TargetToCheck), this);
					break;
				}
				case ETargetingType::Location:
				{
					bIsInCastRange = SkillToCheck->IsInSkillRange_Location(TargetToCheck->GetActorLocation(), this);
					break;
				}
				default:
					break;

				}

				if (bIsInCastRange)
				{
					float DotProductResult = FVector::DotProduct(GetActorForwardVector().GetSafeNormal2D(),
						(TargetToCheck->GetActorLocation() - GetActorLocation()).GetSafeNormal2D());
					if (DotProductResult > HighestCosValue)
					{
						NearestUnit = TargetToCheck;
						HighestCosValue = DotProductResult;
					}
				}
			}
		}
		return NearestUnit;
	}
	return nullptr;

}

float AKRVCombatUnit::GetMaxEngageRange() const
{
	if (GetBaseStat() && GetModifiedStat() && GetBaseStat()->RangedMediumType != ERangedMediumType::NoWeapon)
	{
		return FMath::Max3(GetModifiedStat()->RangedAttackStat.AttackRange, MaxSkillCastRange, 0.f);
	}
	else
	{
		return MaxSkillCastRange;
	}

}

bool AKRVCombatUnit::CastSkill(const FGameplayTag& SkillTagToCast, AKRVCombatUnit* NewUnit /*= nullptr*/, const FVector& NewPoint /*= FVector::ZeroVector*/)
{
	int32 SkillIndexToCast = GetSkillIndexByTag(SkillTagToCast);
	if (!Skills.IsValidIndex(SkillIndexToCast))
	{
		KRVCHECK(Skills.IsValidIndex(SkillIndexToCast));
	}
	else if (!IsSkillReady(SkillTagToCast))
	{
		KRVLOG(Log, TEXT("Skill %s is not ready"), *SkillTagToCast.ToString());
	}
	else if (!IsAbleToAttack())
	{
		// Reserve Skill casting
		FKRVReservedAction SkillCastingToReseve;
		SkillCastingToReseve.ActionStatus = EActionStatus::Casting;
		SkillCastingToReseve.SkillTagToCast = SkillTagToCast;
		SkillCastingToReseve.TargetUnit = NewUnit;
		SkillCastingToReseve.TargetPoint = NewPoint;
		ReserveAction(SkillCastingToReseve);
	}
	else if (!IsInSkillCastRange(SkillTagToCast, NewUnit, NewPoint))
	{
		KRVLOG(Log, TEXT("Taget is out of range to cast %s"), *SkillTagToCast.ToString());
	}
	else if (!PlayCastingMontage(GetCurrentWeaponType()))
	{
		KRVLOG(Error, TEXT("Couldn't play montage"));
		return false;
	}
	else
	{
		AKRVSkill* SkillToCast = Skills[SkillIndexToCast];
		if (!SkillToCast)
		{
			KRVCHECK(SkillToCast);
		}
		else
		{
			// Keep data to cast when casting time ends.
			// 캐스팅 타임이 끝날 때 필요한 데이터 저장
			ETargetingType SkillTargetType = SkillToCast->GetTargetingType();
			ReservedSkillIndexToCast = SkillIndexToCast;
			ReservedSkillTagToCast = SkillTagToCast;
			switch (SkillTargetType)
			{
			case ETargetingType::Target:
			{
				if (NewUnit)
				{
					ReservedUnitToCast = NewUnit;
				}
				else
				{
					ReservedUnitToCast = GetTargetUnit();
				}
				break;
			}
			case ETargetingType::Location:
			{
				if (NewPoint == FVector::ZeroVector)
				{
					ReservedPointToCast = GetTargetPoint();
				}
				else
				{
					ReservedPointToCast = NewPoint;
				}
				break;
			}
			default:
				break;
			}

			// Set status
			SetActionStatus(EActionStatus::Casting);
			SetCombatStatus(ECombatStatus::Ranged);

			// begin casting
			GetWorldTimerManager().ClearTimer(SkillCastingTimer);
			if (SkillToCast->GetCastingTime() > 0.f)
			{
				GetWorldTimerManager().SetTimer(SkillCastingTimer, this, &AKRVCombatUnit::OnCastingTimeUp,
					SkillToCast->GetCastingTime(), false);
			}
			else
			{
				OnCastingTimeUp();
			}

			OnSkillCastBeginDelegate.Broadcast(SkillToCast, this);
			return true;
		}
	}
	return false;

}

FGameplayTag AKRVCombatUnit::GetReservedSkillTag() const
{
	if (!Skills.IsValidIndex(ReservedSkillIndexToCast))
	{
		KRVCHECK(Skills.IsValidIndex(ReservedSkillIndexToCast));
	}
	else
	{
		return ReservedSkillTagToCast;
	}
	return FGameplayTag();

}

bool AKRVCombatUnit::GetSkillTagByIndex(FGameplayTag& OutSkillTag, int32 SkillIndexToGet)
{
	const FGameplayTag* FoundSkillTagPtr = SkillMap.FindKey(SkillIndexToGet);
	if (!FoundSkillTagPtr)
	{
		KRVCHECK(FoundSkillTagPtr);
	}
	else
	{
		OutSkillTag = *FoundSkillTagPtr;
		return true;
	}
	return false;

}

AKRVSkill* AKRVCombatUnit::GetSkillByTag(const FGameplayTag& SkillTagToGet) const
{
	int32 SkillIndexToGet = GetSkillIndexByTag(SkillTagToGet);
	if (!Skills.IsValidIndex(SkillIndexToGet))
	{
		KRVCHECK(Skills.IsValidIndex(SkillIndexToGet));
	}
	else
	{
		return Skills[SkillIndexToGet];
	}
	return nullptr;

}

bool AKRVCombatUnit::SetReservedSkill(const FGameplayTag& SkillTagToSet)
{
	int32 SkillIndexToSet = GetSkillIndexByTag(SkillTagToSet);
	if (!Skills.IsValidIndex(SkillIndexToSet))
	{
		KRVCHECK(Skills.IsValidIndex(SkillIndexToSet));
	}
	else
	{
		ReservedSkillTagToCast = SkillTagToSet;
		ReservedSkillIndexToCast = SkillIndexToSet;
		return true;
	}
	return false;

}

float AKRVCombatUnit::GetMaxSkillRange() const
{
	return MaxSkillCastRange;

}

bool AKRVCombatUnit::AcquireSkill(TSubclassOf<AKRVSkill> NewSkill, int32 LevelToIncrease /*= 1*/, bool bDoesIncreaseFromZero /*= false*/)
{
	if (!NewSkill)
	{
		KRVCHECK(NewSkill);
	}
	else
	{
		AKRVSkill* SkillToAcquire = NewObject<AKRVSkill>(this, NewSkill);
		if (!SkillToAcquire)
		{
			KRVCHECK(SkillToAcquire);
		}
		else
		{
			SkillToAcquire->LoadFromDataAsset_Implementation();
			if (!SkillToAcquire->GetDataAsset_Implementation())
			{
				KRVCHECK(SkillToAcquire->GetDataAsset_Implementation());
			}
			else
			{
				bool bDidAcquireSkill = false;
				int32 AcquiredSkillIndex = -1;
				FGameplayTagContainer NewSkillTagContainer;
				SkillToAcquire->GetDataAsset_Implementation()->GetOwnedGameplayTags(NewSkillTagContainer);
				FGameplayTag SkillNameTag = UKRVBlueprintFunctionLibrary::GetGameplayTagByName(NewSkillTagContainer, "Name.Skill");

				// If it has the new skill already, level up the old skill by the new skill level.
				// 이미 스킬을 가지고 있다면, 해당 레벨만큼 레벨업 시킴.
				for (int32 SkillIndex = 0; SkillIndex < Skills.Num(); ++SkillIndex)
				{
					AKRVSkill* SkillToCheck = Skills[SkillIndex];
					if (SkillToCheck->GetDataAsset_Implementation() &&
						SkillToCheck->GetDataAsset_Implementation()->TagContainer.HasTag(SkillNameTag))
					{
						AcquiredSkillIndex = SkillIndex;
						if (bDoesIncreaseFromZero)
						{
							SkillToCheck->SetMasteryLevel(LevelToIncrease);
							if (LevelToIncrease > 0)
							{
								bDidAcquireSkill = true;
							}
						}
						else
						{
							bDidAcquireSkill = SkillToCheck->MasteryLevelUp(LevelToIncrease);
						}
					}
				}

				// Register the skill to owner.
				if (AcquiredSkillIndex == -1)
				{
					SkillToAcquire->SetOwner(this);
					SkillToAcquire->RegisterAllComponents();
					SkillToAcquire->LoadFromDataAsset_Implementation();
					SkillToAcquire->SetMasteryLevel(SkillToAcquire->GetMasteryLevel() + LevelToIncrease - 1);
					AcquiredSkillIndex = Skills.Emplace(SkillToAcquire);
					SkillMap.Emplace(SkillNameTag, AcquiredSkillIndex);
					bDidAcquireSkill = true;
				}

				if (bDidAcquireSkill)
				{
					// Set max skill cast range or bind passive skill
					switch (SkillToAcquire->GetTargetingType())
					{
					case ETargetingType::Self:
						MaxSkillCastRange = FMath::Max3(MaxSkillCastRange, SkillToAcquire->GetSkillRadius(), 0.f);
						break;
					case ETargetingType::Target:
						MaxSkillCastRange = FMath::Max3(MaxSkillCastRange, SkillToAcquire->GetSkillRange(), 0.f);
						break;
					case ETargetingType::Location:
						MaxSkillCastRange = FMath::Max3(MaxSkillCastRange, SkillToAcquire->GetSkillRange(), 0.f);
						break;
					case ETargetingType::Passive:
						SkillToAcquire->BindPassiveSkill(this);
						break;
					default:
						break;
					}

					OnSkillAcquireDelegate.Broadcast(Skills[AcquiredSkillIndex], this);
				}
				return bDidAcquireSkill;
			}
		}
	}
	return false;

}

bool AKRVCombatUnit::SetSkillLevels(const TArray<FKRVSkillMasterySaveData>& NewSkillMasterySaveData)
{
	bool bSkillLevelSetResult = true;
	if (NewSkillMasterySaveData.Num() < Skills.Num())
	{
		bSkillLevelSetResult = false;
	}

	int32 InitSkillToCheckIndex = 0;
	for (const FKRVSkillMasterySaveData& SkillMasterySaveDataToCheck : NewSkillMasterySaveData)
	{
		bool bIsTheSkillLevelSet = false;
		for (int32 Index = InitSkillToCheckIndex; Index < Skills.Num(); ++Index)
		{
			AKRVSkill* SkillToSetLevel = Skills[Index];
			if (!SkillToSetLevel || !SkillToSetLevel->GetDataAsset_Implementation())
			{
				bSkillLevelSetResult = false;
			}
			else if (SkillToSetLevel->GetDataAsset_Implementation()->ClassFName == SkillMasterySaveDataToCheck.SkillName)
			{
				SkillToSetLevel->SetMasteryLevel(SkillMasterySaveDataToCheck.MasteryLevel);
				bIsTheSkillLevelSet = true;
				if (InitSkillToCheckIndex == Index)
				{
					++InitSkillToCheckIndex;
				}
				break;
			}
		}

		if (!bIsTheSkillLevelSet)
		{
			bSkillLevelSetResult = false;
		}
	}
	return bSkillLevelSetResult;

}

void AKRVCombatUnit::OnCastingTimeUp()
{
	if (!PlayAttackMontage(GetCurrentWeaponType(), 0.5f))
	{
		KRVLOG(Error, TEXT("Couldn't play montage"));
	}

}

void AKRVCombatUnit::OnSkillCasted()
{
	OnAttackCooldownTimeUp();

	if (ReservedSkillIndexToCast >= Skills.Num())
	{
		KRVCHECK(ReservedSkillIndexToCast < Skills.Num());
	}
	else
	{
		AKRVCombatUnit* UnitToCastSkill = nullptr;
		if (ReservedUnitToCast.IsValid())
		{
			UnitToCastSkill = ReservedUnitToCast.Get();
		}

		if (!IsInSkillCastRange(ReservedSkillTagToCast, UnitToCastSkill, ReservedPointToCast))
		{
			ReservedUnitToCast = nullptr;
			ReservedPointToCast = FVector::ZeroVector;
		}
		else
		{
			AKRVSkill* SkillToCast = Skills[ReservedSkillIndexToCast];
			switch (SkillToCast->GetTargetingType())
			{
			case ETargetingType::Self:
			{
				SkillToCast->CastToSelf(this);
				break;
			}
			case ETargetingType::Target:
			{
				if (ReservedUnitToCast.IsValid())
				{
					SkillToCast->CastToTarget(ReservedUnitToCast.Get(), this);
				}
				break;
			}
			case ETargetingType::Location:
			{
				SkillToCast->CastToLocation(ReservedPointToCast, this);
				break;
			}
			default:
				break;
			}
			OnSkillCastEndDelegate.Broadcast(SkillToCast, this);
		}
	}

}

bool AKRVCombatUnit::Die()
{
	if (GetActionStatus() != EActionStatus::Dead)
	{
		UpdateStat();
		SetActorEnableCollision(false);
		RemoveAllBuffsWithStatUpdate(ETargetType::AnyUnit);

		ClearAllActiveActionTimer();
		ClearAllPassiveActionTimer();

		OnSelected(false);
		SetCombatStatus(ECombatStatus::Idle);
		SetActionStatus(EActionStatus::Dead);
		ClearTargetUnit();
		PlayBrainComponent(false);

		if (CombatUnitWidget)
		{
			CombatUnitWidget->SetHiddenInGame(true);
		}

		KRVCHECK(GetKRVAnimInstance());
		if (GetKRVAnimInstance())
		{
			if (!PlayDieMontage(GetCurrentWeaponType()))
			{
				KRVLOG(Error, TEXT("Couldn't play montage"));
			}
		}
		DeathParticleComponent->ActivateSystem(true);
		DeathAudioComponent->Play();

		OnDieDelegate.Broadcast(this);
		GetWorldTimerManager().SetTimer(DeadbodyLifeSpanTimer, this,
			&AKRVCharacter::DestroyThis, DeadbodyLifeSpan, false);
		return true;
	}
	else
	{
		DestroyThis();
		return false;
	}

}

void AKRVCombatUnit::PlayGoldPopUpEffect()
{
	KRVCHECK(GetWorld());
	if (GetWorld() && CoinParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), CoinParticle,
			FTransform(FRotator::ZeroRotator, GetMesh()->GetSocketLocation(OverHeadSocket)), true);
	}

}

void AKRVCombatUnit::PlayRubyPopUpEffect()
{
	KRVCHECK(GetWorld());
	if (GetWorld() && RubyParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RubyParticle,
			FTransform(FRotator::ZeroRotator, GetMesh()->GetSocketLocation(OverHeadSocket)), true);
	}

}
void AKRVCombatUnit::OnUpgrade()
{
	KRVCHECK(GetWorld());
	if (GetWorld() && UpgradeParticle)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UpgradeParticle,
			FTransform(FRotator::ZeroRotator, GetMesh()->GetSocketLocation(LeftShieldSocket)), true);
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), UpgradeParticle,
			FTransform(FRotator::ZeroRotator, GetMesh()->GetSocketLocation(RightShieldSocket)), true);
	}
	KRVCHECK(GetKRVAnimInstance());
	if (GetKRVAnimInstance())
	{
		if (!PlayVictoryMontage(GetCurrentWeaponType()))
		{
			KRVLOG(Error, TEXT("Couldn't play montage"));
		}
	}

}

void AKRVCombatUnit::DestroyThis()
{
	PooledComponent->DeactivateNearActor();

}
