// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVTower.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "WidgetComponent.h"

#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/DecalComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"

#include "KRV.h"
#include "KRVEnums.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Objects/KRVDamageType.h"
#include "DataAssets/KRVTowerDataAsset.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"

#include "Controllers/KRVPlayerAIController.h"
#include "Controllers/KRVEnemyAIController.h"
#include "ActorComponents/KRVStatComponent.h"
#include "ActorComponents/KRVTowerStatComponent.h"
#include "SceneComponents/PooledComponent.h"
#include "SceneComponents/SpawnerComponent.h"
#include "Actors/KRVBuff.h"
#include "Actors/KRVProjectile.h"
#include "Actors/KRVBuildingSite.h"
#include "Pawns/KRVCharacter.h"

#include "GameModes/KRVInGameGameMode.h"
#include "KRVGameState.h"
#include "UIs/KRVCombatUnitUserWidget.h"
#include "UIs/KRVKillRewardUserWidget.h"
#include "UIs/KRVTowerMenuUserWidget.h"


// Sets default values
AKRVTower::AKRVTower()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->InitCapsuleSize(125.f, 160.f);
	GetCharacterMovement()->MaxWalkSpeed = 0.f;

	PlatformMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PLATFORM_MESH_COMPONENT"));
	PlatformMeshComponent->SetupAttachment(RootComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_SPAWNED_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Breakables/barrel/P_BarrelBreak_Wood.P_BarrelBreak_Wood"));
	if (ON_SPAWNED_PARTICLE_OBJ.Succeeded())
	{
		OnSpawnedParticleComponent->SetTemplate(ON_SPAWNED_PARTICLE_OBJ.Object);
		OnSpawnedParticleComponent->AddRelativeLocation(FVector(0.f, 0.f, -230.f));
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> ON_SPAWNED_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Explosion/Cues/retro_explosion_short_10_Cue.retro_explosion_short_10_Cue"));
	if (ON_SPAWNED_SOUND_OBJ.Succeeded())
	{
		OnSpawnedAudioComponent->SetSound(ON_SPAWNED_SOUND_OBJ.Object);
	}
	static ConstructorHelpers::FObjectFinder<UParticleSystem> DEATH_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Cines/Ausar/P_CIN_Ausar_Player_Impact.P_CIN_Ausar_Player_Impact"));
	if (DEATH_PARTICLE_OBJ.Succeeded())
	{
		DeathParticleComponent->SetTemplate(DEATH_PARTICLE_OBJ.Object);
		DeathParticleComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, BackShieldSocket);
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> DEATH_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Explosion/Cues/retro_explosion_deep_13_Cue.retro_explosion_deep_13_Cue"));
	if (DEATH_SOUND_OBJ.Succeeded())
	{
		DeathAudioComponent->SetSound(DEATH_SOUND_OBJ.Object);
	}

	SpawnerComponent = CreateDefaultSubobject<USpawnerComponent>(TEXT("SPAWNER_COMPONENT"));
	SpawnerComponent->SetupAttachment(GetMesh());
	SpawnerComponent->SpawnedActorLifeSpan = 0.f;
	DeadbodyLifeSpan = 20.f;

	// UI
	CombatUnitWidget->AddRelativeLocation(100.f * FVector::UpVector);
	KillRewardWidget->AddRelativeLocation(100.f * FVector::UpVector);

	DecalLocation = FVector(0.f, 0.f, -300.f);
	SelectionDecal->DecalSize.Y = 1.8f * GetCapsuleComponent()->GetScaledCapsuleRadius();
	SelectionDecal->DecalSize.Z = 1.8f * GetCapsuleComponent()->GetScaledCapsuleRadius();
	SelectionDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
	SelectionDecal->SetRelativeLocation(DecalLocation);

	TowerMenuWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("TOWER_MENU_WIDGET"));
	TowerMenuWidget->SetupAttachment(GetRootComponent());

	BaseTowerStat = CreateDefaultSubobject<UKRVTowerStatComponent>(TEXT("TOWER_BASE_STAT"));
	ModifiedTowerStat = CreateDefaultSubobject<UKRVTowerStatComponent>(TEXT("TOWER_MODIFIED_STAT"));

	if (FireRadiusDecal)
	{
		FireRadiusDecal->SetRelativeLocation(DecalLocation);
	}

	WaypointParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("WAYPOINT_PARTICLE_COMPONENT"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> WAYPOINT_PARTICLE_OBJ
	(TEXT("/Game/Resources/Particles/P_Waypoint_1.P_Waypoint_1"));
	if (WAYPOINT_PARTICLE_OBJ.Succeeded())
	{
		WaypointParticleComponent->SetupAttachment(RootComponent);
		WaypointParticleComponent->SetTemplate(WAYPOINT_PARTICLE_OBJ.Object);
		WaypointParticleComponent->SetAutoActivate(false);
	}

	WaypointRadiusDecal = CreateDefaultSubobject<UDecalComponent>(TEXT("WAYPOINT_RANGE_DECAL_COMPONENT"));
	WaypointRadiusDecal->SetupAttachment(GetMesh());
	static ConstructorHelpers::FObjectFinder<UMaterialInstance> WAYPOINT_RANGE_MI_OBJ
	(TEXT("/Game/Resources/Materials/MAT_D_SelectionCircle_Green.MAT_D_SelectionCircle_Green"));
	if (WAYPOINT_RANGE_MI_OBJ.Succeeded())
	{
		WaypointRadiusDecal->SetDecalMaterial(WAYPOINT_RANGE_MI_OBJ.Object);
		WaypointRadiusDecal->SetRelativeRotation(FRotator(90.f, 0.f, 0.f));
		WaypointRadiusDecal->SetRelativeLocation(DecalLocation);
	}
	WaypointRadiusDecal->SetHiddenInGame(true);

}

// Called when the game starts or when spawned
void AKRVTower::BeginPlay()
{
	Super::BeginPlay();

	//PlatformMesh->SetRelativeLocation
	//(GetActorLocation() + PlatformMesh->GetSocketLocation(RootSocket) - PlatformMesh->GetSocketLocation(TopSocket));
	//GetCapsuleComponent()->SetCapsuleHalfHeight
	//(FVector::Distance(PlatformMesh->GetSocketLocation(RootSocket),
	//	GetMesh()->GetSocketLocation(OverHeadSocket)) / 2);

}

// Called every frame
void AKRVTower::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AKRVTower::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	//SpawnerComponent->SetWorldRotation(GetActorRotation());

	AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(this);
	if (KRVInGameGameMode)
	{
		OnRecrutiedDelegate.AddUObject(KRVInGameGameMode, &AKRVInGameGameMode::OnRecruited);
	}

}

void AKRVTower::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	SetTeam(EKRVTeam::Player);

}

UKRVTowerDataAsset* AKRVTower::GetTowerDataAsset() const
{
	return Cast<UKRVTowerDataAsset>(GetDataAsset_Implementation());

}

bool AKRVTower::LoadFromDataAsset_Implementation()
{
	if (!Super::LoadFromDataAsset_Implementation())
	{
		KRVCHECK(GetTowerDataAsset());
		return false;
	}

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	PlatformMeshPath = GetTowerDataAsset()->PlatformMesh.ToSoftObjectPath();
	Streamable.RequestAsyncLoad(PlatformMeshPath, FStreamableDelegate::CreateUObject
	(this, &AKRVTower::OnPlatformMeshAsyncLoaded));

	{
		SpawnerComponent->SetPoolWithCharacterSpawnData(GetTowerDataAsset()->CharacterSpawnData);
		/*SpawnerComponent->SetSpawnParams(FVector(200.f, 0.f, -80.f), FRotator::ZeroRotator, true);
		SpawnerComponent->SetBoxSize(FVector(100.f, 100.f, 1.f));*/
		SpawnerComponent->GetPool_Template<AKRVCharacter>(RecruitsInPool);
		for (AKRVCharacter* KRVCharacter : RecruitsInPool)
		{
			KRVCHECK(KRVCharacter);
			if (KRVCharacter)
			{
				KRVCharacter->OnDieDelegate.AddUObject(this, &AKRVTower::OnRecruitDie);
			}
		}
	}

	for (auto SoftPtr_TowerUpgrade : GetTowerDataAsset()->TowerUpgrades)
	{
		UKRVTowerUpgradeDataAsset* HardPtr_TowerUpgradeDA =
			Cast<UKRVTowerUpgradeDataAsset>(Streamable.LoadSynchronous(SoftPtr_TowerUpgrade.ToSoftObjectPath(), true));
		if (HardPtr_TowerUpgradeDA)
		{
			TowerUpgradeDAs.Emplace(HardPtr_TowerUpgradeDA);
		}
	}
	TowerUpgradeLevels.SetNum(TowerUpgradeDAs.Num());

	return true;

}

void AKRVTower::OnPlatformMeshAsyncLoaded()
{
	TSoftObjectPtr<UStaticMesh> AssetPathPtr(PlatformMeshPath);
	if (AssetPathPtr.IsValid())
	{
		PlatformMeshComponent->SetCollisionProfileName("NoCollision");

		PlatformMeshComponent->SetStaticMesh(AssetPathPtr.Get());
		PlatformMeshComponent->SetWorldScale3D(FVector(0.6f));
		PlatformMeshComponent->AddLocalRotation(FRotator(0.f, 180.f, 0.f));
		PlatformMeshComponent->AddWorldOffset(FVector(5.f, 5.f, -160.f));
		PlatformMeshComponent->SetAbsolute(false, true, false);

		GetMesh()->AddWorldOffset((FVector::Dist(PlatformMeshComponent->GetComponentLocation(), PlatformMeshComponent->GetSocketLocation(TopSocket))
			/ 2 - 60.f) * FVector::UpVector);
	}

}

void AKRVTower::OnTowerMenuWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVTowerMenuUserWidget> AssetPathPtr(TowerMenuWidgetClassPath);
	if (AssetPathPtr.IsValid())
	{
		TowerMenuWidget->SetWidgetClass(AssetPathPtr.Get());
		TowerMenuWidget->SetWidgetSpace(EWidgetSpace::Screen);
		TowerMenuWidget->AddRelativeLocation(0.f * FVector::UpVector);
		TowerMenuWidget->SetDrawSize(FVector2D(160.f, 160.f));
		TowerMenuWidget->SetHiddenInGame(false);

		UKRVTowerMenuUserWidget* CastedTowerMenuWidget = Cast<UKRVTowerMenuUserWidget>(TowerMenuWidget->GetUserWidgetObject());
		if (CastedTowerMenuWidget)
		{
			CastedTowerMenuWidget->BindToTower(this);
			CastedTowerMenuWidget->SetVisibility(ESlateVisibility::Hidden);
		}
	}

}

bool AKRVTower::LoadBaseStat()
{
	if (!GetDataAsset_Implementation() || !GetBaseTowerStat() || !GetModifiedTowerStat())
	{
		return false;
	}
	if (GetBaseTowerStat()->LoadData(GetDataAsset_Implementation()->ClassFName) &&
		GetModifiedTowerStat()->LoadData(GetDataAsset_Implementation()->ClassFName))
	{
		return true;
	}
	return false;


}


UKRVTowerStatComponent* AKRVTower::GetBaseTowerStat() const
{
	return BaseTowerStat;

}

UKRVTowerStatComponent* AKRVTower::GetModifiedTowerStat() const
{
	return ModifiedTowerStat;

}

void AKRVTower::OnBeginPlayAndSpawned()
{
	Super::OnBeginPlayAndSpawned();
	TowerMenuWidget->SetHiddenInGame(false);
	SetRecruitWaypoint(GetActorLocation() + 400 * GetActorForwardVector() - 50.f * FVector::DownVector);

	UKRVTowerMenuUserWidget* CastedTowerMenuWidget = Cast<UKRVTowerMenuUserWidget>(TowerMenuWidget->GetUserWidgetObject());
	KRVCHECK(CastedTowerMenuWidget);
	if (CastedTowerMenuWidget)
	{
		CastedTowerMenuWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}

void AKRVTower::OnSpawned()
{
	Super::OnSpawned();

}

void AKRVTower::DestroyThis()
{
	SpawnerComponent->DestroyPool();
	Super::DestroyThis();

}

AKRVBuildingSite* AKRVTower::GetBuildingSite() const
{
	if (BuildingSite.IsValid())
	{
		return BuildingSite.Get();
	}
	else
	{
		KRVCHECK(BuildingSite.IsValid());
		return nullptr;
	}

}

void AKRVTower::SetBuildingSite(AKRVBuildingSite* NewBuildingSite)
{
	BuildingSite = NewBuildingSite;

}

bool AKRVTower::PlayBrainComponent(bool bIsTrue)
{
	if (!Super::PlayBrainComponent(bIsTrue))
	{
		return false;
	}
	else
	{
		Recruit();
		return true;
	}

}

void AKRVTower::BindToWidgetsOnBeginPlay()
{
	Super::BindToWidgetsOnBeginPlay();

	UKRVCombatUnitUserWidget* CastedCombatUnitWidget = Cast<UKRVCombatUnitUserWidget>(CombatUnitWidget->GetUserWidgetObject());
	KRVCHECK(CastedCombatUnitWidget);
	if (CastedCombatUnitWidget)
	{
		CastedCombatUnitWidget->BindToCombatUnit(Cast<AKRVCombatUnit>(this));
	}

}

void AKRVTower::BindToWidgetsBySpawner()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	Streamable.RequestAsyncLoad(TowerMenuWidgetClassPath, FStreamableDelegate::CreateUObject
	(this, &AKRVTower::OnTowerMenuWidgetClassAsyncLoaded));

}

void AKRVTower::OnSelected(bool bIsSelected)
{
	if (bIsCurrentlySelected != bIsSelected)
	{
		Super::OnSelected(bIsSelected);
		ShowWaypointRadiusDecal(bIsSelected);
	}

}

void AKRVTower::OnGameEnd(EKRVTeam Winner)
{
	if (GetActionStatus() != EActionStatus::Dead && GetWorld())
	{
		GetWorldTimerManager().ClearTimer(RecruitRateTimer);
		Super::OnGameEnd(Winner);
	}

}

//bool AKRVTower::TakeBuffWithStatUpdate(AKRVBuff* NewBuff)
//{
//	// Extra condition for Tower
//	if (!NewBuff)
//	{
//		KRVCHECK(NewBuff);
//		return false;
//	}
//	else if (!TakeBuff(NewBuff))
//	{
//		return false;
//	}
//	else
//	{
//		NewBuff->OnThisBuffRemovedThisPtr.BindUObject(this, &AKRVTower::RemoveBuff);
//		UpdateStat();
//		return true;
//	}
//	return false;
//
//}
//
//bool AKRVTower::RemoveBuffWithStatUpdate(AKRVBuff* NewBuff)
//{
//	if (!RemoveBuff(NewBuff))
//	{
//		return false;
//	}
//	else
//	{
//		UpdateStat();
//		return true;
//	}
//
//}

UKRVStatComponent* AKRVTower::VariateBaseStat(TSubclassOf<AKRVBuff> BuffClass)
{
	if (!GetBaseTowerStat())
	{
		KRVCHECK(GetBaseTowerStat());
		return nullptr;
	}
	else
	{
		Ptr_TemporaryBuff = NewObject<AKRVBuff>(this, BuffClass);
		Ptr_TemporaryBuff->SetOwner(this);
		*BaseTowerStat += *Ptr_TemporaryBuff;
		Ptr_TemporaryBuff = nullptr;
		return GetBaseStat();
	}

}

UKRVStatComponent* AKRVTower::GetBaseStat() const
{
	return Cast<UKRVStatComponent>(GetBaseTowerStat());

}

UKRVStatComponent* AKRVTower::GetModifiedStat() const
{
	return Cast<UKRVStatComponent>(GetModifiedTowerStat());

}

void AKRVTower::UpdateStat()
{
	UKRVTowerStatComponent* TowerBaseStat = GetBaseTowerStat();
	UKRVTowerStatComponent* TowerModifiedStat = GetModifiedTowerStat();
	if (!TowerBaseStat || !TowerModifiedStat)
	{
		KRVCHECK(TowerBaseStat);
		KRVCHECK(TowerModifiedStat);
		return;
	}
	else if (TowerBaseStat && TowerModifiedStat)
	{
		TowerModifiedStat->AssignmentOperator(TowerBaseStat);
		*TowerModifiedStat += *UpdateBuffStatSum();
		*TowerModifiedStat *= *BuffSum;

		FireRadiusDecal->DecalSize.Y = GetModifiedTowerStat()->RangedAttackStat.AttackRange;
		FireRadiusDecal->DecalSize.Z = GetModifiedTowerStat()->RangedAttackStat.AttackRange;

		if (SpawnerComponent && SpawnerComponent->IsPoolSet())
		{
			WaypointRadiusDecal->DecalSize.Y = GetBaseTowerStat()->WaypointRadius;
			WaypointRadiusDecal->DecalSize.Z = GetBaseTowerStat()->WaypointRadius;
		}
	}

	OnStatUpdatedDelegate.Broadcast(this);

}

bool AKRVTower::MakeStunned(float NewDuration)
{
	if (!Super::MakeStunned(NewDuration))
	{
		return false;
	}
	else if (GetWorld() && GetWorldTimerManager().GetTimerRemaining(RecruitRateTimer) > 0.f)
	{
		GetWorldTimerManager().PauseTimer(RecruitRateTimer);
	}
	return true;

}

bool AKRVTower::StopAction()
{
	return Super::StopAction();

}

void AKRVTower::OnStunTimeUp()
{
	Super::OnStunTimeUp();

	if (GetActionStatus() != EActionStatus::Dead)
	{
		if (GetWorld() && GetWorldTimerManager().GetTimerRemaining(RecruitRateTimer) > 0.f)
		{
			GetWorldTimerManager().UnPauseTimer(RecruitRateTimer);
		}
		else
		{
			Recruit();
		}
	}

}

bool AKRVTower::Die()
{
	if (Super::Die() && GetWorld())
	{
		GetWorldTimerManager().ClearTimer(RecruitRateTimer);
		if (WasteBuildingLifeSpan >= DeadbodyLifeSpan)
		{
			KRVCHECK(WasteBuildingLifeSpan < DeadbodyLifeSpan);
		}
		else
		{
			GetWorldTimerManager().SetTimer(HideMeshTimer, this, &AKRVTower::HideBodyMesh, WasteBuildingLifeSpan, false);
		}
		return true;
	}
	return false;

}

void AKRVTower::HideBodyMesh()
{
	GetMesh()->SetHiddenInGame(true, true);
	if (PlatformMeshComponent)
	{
		PlatformMeshComponent->SetHiddenInGame(true, true);
	}

}

int32 AKRVTower::GetCurrentRecruits() const
{
	return RecruitsInField.Num();

}

void AKRVTower::OnRecruitDie(class AKRVCombatUnit* DeadUnit)
{
	AKRVCharacter* DeadCharacter = Cast<AKRVCharacter>(DeadUnit);
	if (!DeadCharacter)
	{
		KRVCHECK(DeadCharacter);
	}
	else
	{
		RecruitsInField.Remove(DeadCharacter);
		Recruit();
	}

}

void AKRVTower::OnRecruitComplete()
{
	AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(SpawnerComponent->Spawn());
	KRVCHECK(KRVCharacter);
	if (KRVCharacter)
	{
		KRVCharacter->bIsRecruitedByTower = true;
		KRVCharacter->BePossessedByAIController(GetTeam(), true);
		KRVCharacter->SetTargetPoint(GetRecruitWaypoint());
		KRVCharacter->MoveToTargetPoint();
		RecruitsInField.Emplace(KRVCharacter);
		OnRecrutiedDelegate.Broadcast(KRVCharacter);
	}

	Recruit();

}

bool AKRVTower::Recruit()
{
	if (!IsAbleToRecruit() || !SpawnerComponent)
	{
		KRVCHECK(SpawnerComponent);
		return false;
	}
	else
	{
		GetWorldTimerManager().SetTimer(RecruitRateTimer, this,
			&AKRVTower::OnRecruitComplete, GetTowerDataAsset()->CharacterSpawnData.SpawnRate, false);
		return true;
	}
	return false;

}

bool AKRVTower::IsAbleToRecruit() const
{
	if (!IsRecruitableTower() || !GetWorld())
	{
		return false;
	}
	else if (GetWorldTimerManager().GetTimerRemaining(RecruitRateTimer) > 0.1f ||
		RecruitsInField.Num() >= GetTowerDataAsset()->CharacterSpawnData.CharactersToSpawn)
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool AKRVTower::IsRecruitableTower() const
{
	if (!GetDataAsset() || !SpawnerComponent)
	{
		KRVCHECK(GetDataAsset());
		KRVCHECK(SpawnerComponent);
		return false;
	}
	else if (!IsAbleToMove() || !SpawnerComponent->IsPoolSet())
	{
		KRVCHECK(SpawnerComponent->IsPoolSet());
		return false;
	}
	else
	{
		return true;
	}

}

void AKRVTower::GetPointOnNearestPath(FVector& OutVec) const
{
	if (!BuildingSite.IsValid())
	{
		KRVCHECK(BuildingSite.IsValid());
	}
	else
	{
		BuildingSite.Get()->GetPointOnNearestPath(OutVec);
	}

}

FVector AKRVTower::GetRecruitWaypoint() const
{
	return RecruitWaypoint;

}

bool AKRVTower::SetRecruitWaypoint(const FVector& NewLocation)
{
	if (!GetWorld() || !GetBaseTowerStat())
	{
		KRVCHECK(GetBaseTowerStat());
		KRVCHECK(SpawnerComponent);
		return false;
	}
	else if (!IsRecruitableTower())
	{
		return false;
	}
	else
	{
		if (FVector::DistSquared2D(GetActorLocation(), NewLocation) >
			FMath::Square(GetBaseTowerStat()->WaypointRadius))
		{
			KRVCHECK(FVector::DistSquared2D(GetActorLocation(), NewLocation) <=
				FMath::Square(GetBaseTowerStat()->WaypointRadius));
			return false;
		}
		else
		{
			RecruitWaypoint = NewLocation;
			WaypointParticleComponent->SetWorldLocation(NewLocation + 80.f * FVector::UpVector);
			MoveRecruits(ECombatStatus::ForcedMoving);
			return true;
		}
	}
	return false;
}

bool AKRVTower::MakeRecruitsReturnToTheWaypoint()
{
	if (RecruitsInField.Num() <= 0)
	{
		return false;
	}
	else
	{
		return MoveRecruits(ECombatStatus::FollowingPath);
	}
	return false;

}

bool AKRVTower::MoveRecruits(ECombatStatus NewStatus)
{
	if (RecruitsInField.Num() <= 0)
	{
		return false;
	}
	else if (!(NewStatus == ECombatStatus::FollowingPath || NewStatus == ECombatStatus::ForcedMoving
		|| NewStatus == ECombatStatus::Idle))
	{
		return false;
	}
	else
	{
		bool bReturnResult = true;
		TArray<FVector> Variations;
		UKRVBlueprintFunctionLibrary::MakeSquareFormation(Variations, RecruitsInField.Num(), 150.f);
		KRVCHECK(Variations.Num() == RecruitsInField.Num());
		for (int32 Index = 0; Index < RecruitsInField.Num(); ++Index)
		{
			AKRVCharacter* RecruitToMove = RecruitsInField[Index];
			if (!RecruitToMove)
			{
				bReturnResult = false;
			}
			else
			{
				RecruitToMove->SetTargetPoint(GetRecruitWaypoint() + Variations[Index]);
				bReturnResult &= RecruitToMove->MoveToTargetPoint(NewStatus);
			}
		}
		return bReturnResult;
	}
	return false;

}

void AKRVTower::ShowWaypointRadiusDecal(bool bDoesShow)
{
	if (!SpawnerComponent || !SpawnerComponent->IsPoolSet())
	{
		return;
	}
	else if (WaypointRadiusDecal)
	{
		WaypointRadiusDecal->SetHiddenInGame(!bDoesShow);
		if (bDoesShow)
		{
			WaypointParticleComponent->ActivateSystem();
		}
		else
		{
			WaypointParticleComponent->DeactivateSystem();
		}
	}

}

void AKRVTower::SellThis()
{
	if (!IsSellable())
	{
		return;
	}
	else
	{
		StopAction();
		PlayBrainComponent(false);

		OnSellingBeginDelegate.Broadcast(this);
		if (TowerSellingTime > 0)
		{
			GetWorldTimerManager().SetTimer(TowerUpgradeTimer, this, &AKRVTower::OnSellingFinished, TowerSellingTime, false);
		}
		else
		{
			OnSellingFinished();
		}
	}

}

int32 AKRVTower::GetSellingCost() const
{
	if (!GetBuildingSite())
	{
		KRVCHECK(GetBuildingSite());
		return -1;
	}
	else
	{
		return GetBuildingSite()->GetSellingCost();
	}

}

bool AKRVTower::UpgradeTower(int32 Index)
{
	if (!GetWorld() || !TowerUpgradeDAs.IsValidIndex(Index))
	{
		KRVCHECK(GetWorld());
		KRVCHECK(TowerUpgradeDAs.IsValidIndex(Index));
		return false;
	}
	else if (!IsAbleToUpgradeTower(Index))
	{
		return false;
	}
	else
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		UKRVTowerUpgradeDataAsset* TowerUpgradeDA = TowerUpgradeDAs[Index];

		switch (TowerUpgradeDA->TowerUpgradeType)
		{
		case ETowerUpgradeType::NewTower:
		{
			if (!BuildTowerDelegate.IsBound())
			{
				KRVCHECK(BuildTowerDelegate.IsBound());
				return false;
			}
			else if (!BuildTowerDelegate.Execute(TowerUpgradeDA->NewTower, TowerUpgradeDA->BuildTime))
			{
				KRVLOG(Error, TEXT("New tower was couldn't built. Delegate not exectued."));
				return false;
			}
			else if (!bIsDummyTower)
			{
				OnNewTowerBeginBuilt();
			}
			break;
		}
		default:
			break;
		}

		// Cost
		int32 ActualUpgradeCost = GetActualTowerUpgradeCost(Index);
		IncreaseSellingCostDelegate.ExecuteIfBound(ActualUpgradeCost);
		KRVInGameGameMode->AddAvailableGold(-1 * ActualUpgradeCost);

		// Upgrade begin
		CurrentUpgradeIndex = Index;
		if (TowerUpgradeDA->BuildTime > 0)
		{
			GetWorldTimerManager().SetTimer(TowerUpgradeTimer, this, &AKRVTower::OnUpgradeFinished, TowerUpgradeDA->BuildTime, false);
		}
		else
		{
			OnUpgradeFinished();
		}
		OnUpgradeBeginDelegate.Broadcast(TowerUpgradeDA, this);
		return true;
	}
	return false;

}

void AKRVTower::OnNewTowerBeginBuilt()
{
	StopAction();
	PlayBrainComponent(false);
	SetActorEnableCollision(false);
	HideBodyMesh();

	if (IsRecruitableTower())
	{
		GetWorldTimerManager().PauseTimer(RecruitRateTimer);
		for (int32 Index = 0; Index < RecruitsInField.Num(); ++Index)
		{
			AKRVCharacter* RecruitToDestroy = RecruitsInField[Index];
			if (RecruitToDestroy)
			{
				RecruitToDestroy->DestroyThis();
			}
		}
	}

}

bool AKRVTower::IsAbleToUpgradeTower(int32 Index)
{
	if (!GetWorld() || !TowerUpgradeDAs.IsValidIndex(Index))
	{
		KRVCHECK(GetWorld());
		KRVCHECK(TowerUpgradeDAs.IsValidIndex(Index));
		return false;
	}
	else
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		AKRVGameState* KRVGameState = UKRVBlueprintFunctionLibrary::GetKRVGameState(GetWorld());
		UKRVTowerUpgradeDataAsset* TowerUpgradeDA = TowerUpgradeDAs[Index];
		if (!KRVInGameGameMode || !KRVGameState || !TowerUpgradeDA)
		{
			KRVCHECK(KRVGameState);
			KRVCHECK(KRVInGameGameMode);
			KRVCHECK(TowerUpgradeDA);
			return false;
		}
		else
		{
			int32 AvaialbleGold = KRVGameState->GetAvailableGold();
			float ActualUpgradeCost = GetActualTowerUpgradeCost(Index);
			if (ActualUpgradeCost > AvaialbleGold)
			{
				KRVLOG(Warning, TEXT("Not enougth gold %d / %d"), AvaialbleGold, ActualUpgradeCost);
				return false;
			}
			else if (TowerUpgradeLevels[Index] >= TowerUpgradeDA->MaxUpgradeLevel)
			{
				KRVLOG(Warning, TEXT("Cannot upgrade more than MaxUpgradLevel"));
				return false;
			}
			else
			{
				return true;
			}
		}
	}
	return false;

}

UKRVTowerUpgradeDataAsset* AKRVTower::GetTowerUpgradeDA(int32 Index) const
{
	if (!TowerUpgradeDAs.IsValidIndex(Index))
	{
		KRVCHECK(TowerUpgradeDAs.IsValidIndex(Index));
		return nullptr;
	}
	else
	{
		return TowerUpgradeDAs[Index];
	}

}


int32 AKRVTower::GetIndexByTowerUpgradeDA(class UKRVTowerUpgradeDataAsset* DAToCheck)
{
	return TowerUpgradeDAs.Find(DAToCheck);

}

int32 AKRVTower::GetTowerUpgradeLevel(int32 Index) const
{
	if (!TowerUpgradeLevels.IsValidIndex(Index))
	{
		KRVCHECK(TowerUpgradeLevels.IsValidIndex(Index));
		return -1;
	}
	else
	{
		return TowerUpgradeLevels[Index];
	}


}

int32 AKRVTower::GetActualTowerUpgradeCost(int32 NewDAIndex) const
{
	if (!TowerUpgradeDAs.IsValidIndex(NewDAIndex))
	{
		KRVCHECK(TowerUpgradeDAs.IsValidIndex(NewDAIndex));
		return false;
	}
	else
	{
		UKRVTowerUpgradeDataAsset* TowerUpgradeDA = TowerUpgradeDAs[NewDAIndex];
		float CostScale = 1.f;
		const FGameplayTagContainer& TowerUpgradeDATagContainer = TowerUpgradeDA->TagContainer;
		FGameplayTagContainer DiscountTags;
		for (const auto& TowerUpgradeDiscount : TowerUpgradeDiscounts)
		{
			if (TowerUpgradeDATagContainer.HasTag(TowerUpgradeDiscount.Key))
			{
				CostScale -= TowerUpgradeDiscount.Value;
			}
		}
		CostScale = FMath::Clamp(CostScale, 0.f, 1.f);
		return CostScale * TowerUpgradeDA->UpgradeCost;
	}

}

bool AKRVTower::IsSellable() const
{
	if (!GetTowerDataAsset() || bIsDummyTower)
	{
		KRVCHECK(GetTowerDataAsset());
		KRVCHECK(!bIsDummyTower);
		return false;
	}
	else if (GetTowerDataAsset()->bIsSellable)
	{
		return true;
	}
	return false;

}

float AKRVTower::GetSellingTime() const
{
	return TowerSellingTime;

}

void AKRVTower::AcquireTowerUpgrade(UKRVTowerUpgradeDataAsset* NewTowerUpgradeDA)
{
	KRVCHECK(NewTowerUpgradeDA);
	if (NewTowerUpgradeDA)
	{
		TowerUpgradeDAs.Emplace(NewTowerUpgradeDA);
		TowerUpgradeLevels.Emplace(0);
	}

}

float AKRVTower::GetRemainingUpgradeBuildTime()
{
	if (GetWorld())
	{
		return GetWorldTimerManager().GetTimerRemaining(TowerUpgradeTimer);
	}
	else
	{
		return -1.f;
	}

}

void AKRVTower::OnUpgradeFinished()
{
	if (!GetWorld() || !(CurrentUpgradeIndex >= 0) || !(CurrentUpgradeIndex < TowerUpgradeDAs.Num()))
	{
		KRVCHECK(GetWorld());
		KRVCHECK((CurrentUpgradeIndex >= 0) && (CurrentUpgradeIndex < TowerUpgradeDAs.Num()));
		return;
	}
	else
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		AKRVGameState* KRVGameState = UKRVBlueprintFunctionLibrary::GetKRVGameState(GetWorld());
		UKRVTowerUpgradeDataAsset* TowerUpgradeDA = TowerUpgradeDAs[CurrentUpgradeIndex];
		if (!KRVInGameGameMode || !KRVGameState || !TowerUpgradeDA)
		{
			KRVCHECK(KRVGameState);
			KRVCHECK(KRVInGameGameMode);
			KRVCHECK(TowerUpgradeDA);
			return;
		}
		else
		{
			if (!bIsDummyTower)
			{
				TowerUpgradeLevels[CurrentUpgradeIndex] = TowerUpgradeDA->UpgradeLevelTo;

				switch (TowerUpgradeDA->TowerUpgradeType)
				{
				case ETowerUpgradeType::NewTower:
				{
					Die();
					break;
				}
				case ETowerUpgradeType::TowerStat:
				{
					if (!VariateBaseStat(TowerUpgradeDA->UpgradeStat))
					{
						KRVLOG(Warning, TEXT("Couldn't upgrade tower stat"));
						return;
					}
					else
					{
						if (TowerUpgradeDA->GearUpgradePart == EGearUpgradePart::Ranged)
						{
							LoadRangedWeapon(TowerUpgradeDA->GearUpgradeIndex);
						}
						OnUpgrade();
					}
					break;
				}
				case ETowerUpgradeType::CharacterStat:
				{
					if (RecruitsInPool.Num() <= 0)
					{
						KRVCHECK(RecruitsInPool.Num() > 0);
						return;
					}
					else
					{
						for (AKRVCharacter* KRVCharacter : RecruitsInPool)
						{
							if (!KRVCharacter)
							{
								KRVCHECK(KRVCharacter);
							}
							else
							{
								if (!KRVCharacter->VariateBaseStat(TowerUpgradeDA->UpgradeStat))
								{
									KRVLOG(Error, TEXT("Couldn't upgrade the tower stat"));
								}
								else
								{
									switch (TowerUpgradeDA->GearUpgradePart)
									{
									case EGearUpgradePart::Ranged:
									{
										KRVCharacter->LoadRangedWeapon(TowerUpgradeDA->GearUpgradeIndex);
										break;
									}
									case EGearUpgradePart::LeftMelee:
									{
										KRVCharacter->LoadMeleeWeapon(true, TowerUpgradeDA->GearUpgradeIndex);
										break;
									}
									case EGearUpgradePart::RightMelee:
									{
										KRVCharacter->LoadMeleeWeapon(false, TowerUpgradeDA->GearUpgradeIndex);
										break;
									}
									default:
										break;
									}
									KRVCharacter->OnUpgrade();
								}
							}
						}
					}
					break;
				}
				case ETowerUpgradeType::TowerSkill:
				{
					if (!AcquireSkill(TowerUpgradeDA->UpgradeSkill))
					{
						KRVLOG(Error, TEXT("The tower couldn't acquire the skill"));
						return;
					}
					else
					{
						OnUpgrade();
					}
					break;
				}
				case ETowerUpgradeType::CharacterSkill:
				{
					if (RecruitsInPool.Num() <= 0)
					{
						KRVCHECK(RecruitsInPool.Num() > 0);
						return;
					}
					else
					{
						for (AKRVCharacter* KRVCharacter : RecruitsInPool)
						{
							if (!KRVCharacter)
							{
								KRVCHECK(KRVCharacter);
							}
							else
							{
								if (!KRVCharacter->AcquireSkill(TowerUpgradeDA->UpgradeSkill))
								{
									KRVLOG(Error, TEXT("The charater couldn't acquire the skill"));
								}
								else
								{
									KRVCharacter->OnUpgrade();
								}
							}
						}
					}
					break;
				}
				default:
					break;
				}

			}

			OnUpgradeFinishedDelegate.Broadcast(TowerUpgradeDA, this);

			//Replace
			if (!bIsDummyTower && TowerUpgradeDA->UpgradeLevelTo < TowerUpgradeDA->MaxUpgradeLevel)
			{
				FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
				UKRVTowerUpgradeDataAsset* NewDAToReplacedWith = Cast<UKRVTowerUpgradeDataAsset>
					(Streamable.LoadSynchronous(TowerUpgradeDA->NextTowerUpgradeDA.ToSoftObjectPath(), true));
				TowerUpgradeDAs[CurrentUpgradeIndex] = NewDAToReplacedWith;
			}
		}
	}

}

void AKRVTower::OnSellingFinished()
{
	OnSellingFinishedDelegate.Broadcast(this);
	Die();

}

