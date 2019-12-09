// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVBuildingSite.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/Material.h"
#include "Components/SceneComponent.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Components/CapsuleComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVTowerDataAsset.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"

#include "SceneComponents/PooledComponent.h"
#include "Pawns/KRVTower.h"
#include "KRVPath.h"
#include "GameModes/KRVInGameGameMode.h"
#include "PlayerStates/KRVInGamePlayerState.h"

AKRVBuildingSite::AKRVBuildingSite()
{
	PrimaryActorTick.bCanEverTick = false;

	SiteBaseMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SITE_BASE_MESH_COMPONENT"));
	SetRootComponent(SiteBaseMeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SITE_BASE_MESH_OBJ(TEXT
	("/Game/Resources/StaticMeshes/SM_BuildingSite_1.SM_BuildingSite_1"));
	if (SITE_BASE_MESH_OBJ.Succeeded())
	{
		SiteBaseMeshComponent->SetStaticMesh(SITE_BASE_MESH_OBJ.Object);
		SiteBaseMeshComponent->SetCollisionProfileName("NoCollision");
	}
	SetActorRotation(FRotator(0.f, 180.f, 0.f));

	FlagMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("FLAG_MESH_COMPONENT"));
	FlagMeshComponent->SetupAttachment(SiteBaseMeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> FLAG_MESH_OBJ(TEXT
	("/Game/Resources/StaticMeshes/SM_Flag_1.SM_Flag_1"));
	if (FLAG_MESH_OBJ.Succeeded())
	{
		FlagMeshComponent->SetStaticMesh(FLAG_MESH_OBJ.Object);
		FlagMeshComponent->SetCollisionProfileName("NoCollision");
		FlagMeshComponent->AddRelativeRotation(FRotator(0.f, 90.f, 0.f));
		FlagMeshComponent->SetWorldScale3D(FVector(2.f));
	}

	UnderConstructionMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("UNDER_CONSTRUCTION_MESH"));
	UnderConstructionMeshComponent->SetupAttachment(SiteBaseMeshComponent);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> UNDER_CONSTRUCTION_MESH_OBJ(TEXT
	("/Game/Resources/StaticMeshes/SM_UnderConstrunction_1.SM_UnderConstrunction_1"));
	if (UNDER_CONSTRUCTION_MESH_OBJ.Succeeded())
	{
		UnderConstructionMeshComponent->SetStaticMesh(UNDER_CONSTRUCTION_MESH_OBJ.Object);
		UnderConstructionMeshComponent->SetCollisionProfileName("NoCollision");
		UnderConstructionMeshComponent->SetWorldScale3D(FVector(0.6f, 0.6f, 0.6f));
		UnderConstructionMeshComponent->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));
		UnderConstructionMeshComponent->SetVisibility(false, false);
	}


	OnBuildStartParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("ON_BUILD_START_PARTICLE_COMPONENT"));
	OnBuildStartParticleComponent->SetupAttachment(SiteBaseMeshComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_BUILD_START_PARTICLE_OBJ(TEXT
	("/Game/Resources/Particles/P_BuildingDust_1.P_BuildingDust_1"));
	if (ON_BUILD_START_PARTICLE_OBJ.Succeeded())
	{
		OnBuildStartParticleComponent->SetTemplate(ON_BUILD_START_PARTICLE_OBJ.Object);
		OnBuildStartParticleComponent->SetAutoActivate(false);
	}

	OnBuildStartAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("ON_BUILD_START_AUDIO_COMPONENT"));
	OnBuildStartAudioComponent->SetupAttachment(SiteBaseMeshComponent);
	static ConstructorHelpers::FObjectFinder<USoundCue> ON_BUILD_START_AUDIO_OBJ(TEXT
	("/Game/Retro_8Bit_Sounds/Explosion/Cues/retro_explosion_short_05_Cue.retro_explosion_short_05_Cue"));
	if (ON_BUILD_START_AUDIO_OBJ.Succeeded())
	{
		OnBuildStartAudioComponent->SetSound(ON_BUILD_START_AUDIO_OBJ.Object);
		OnBuildStartAudioComponent->SetAutoActivate(false);
	}

	SetActorEnableCollision(false);

}

void AKRVBuildingSite::PostInitializeComponents()
{
	Super::PostInitializeComponents();

}

void AKRVBuildingSite::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
		if (!KRVInGamePlayerState)
		{
			KRVCHECK(KRVInGamePlayerState);
		}
		else
		{
			{ // Load Upgrade
				TArray<FName> EquippedTowerNames;
				KRVInGamePlayerState->GetEquippedTowerNames(EquippedTowerNames);
				for (const FName& TowerName : EquippedTowerNames)
				{
					FString TowerUpgradePathString = UpgradePath + UpgradePrefix + TowerName.ToString() + UpgradePostfix +
						FString(TEXT(".")) + UpgradePrefix + TowerName.ToString() + UpgradePostfix;
					FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
					UKRVTowerUpgradeDataAsset* LoadedTowerUpgradeDA = Cast<UKRVTowerUpgradeDataAsset>
						(Streamable.LoadSynchronous(FSoftObjectPath(TowerUpgradePathString), true));
					if (LoadedTowerUpgradeDA)
					{
						UpgradeDataAssets.Emplace(LoadedTowerUpgradeDA);
					}
				}
			}

			if (UpgradeDataAssets.Num() == 0)
			{
				KRVCHECK(UpgradeDataAssets.Num() > 0);
			}
			else
			{
				// Spawn DummyTower
				FActorSpawnParameters SpawnParameters;
				SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
				DummyTower = GetWorld()->SpawnActor<AKRVTower>
					(AKRVTower::StaticClass(), GetActorTransform(), SpawnParameters);
				InitTowerSet(DummyTower);
				DummyTower->bIsDummyTower = true;
				DummyTower->SetActorHiddenInGame(true);

				// Give TowerUpgrades To DummyTower
				for (int32 Index = UpgradeDataAssets.Num() - 1; Index >= 0; --Index)
				{
					if (!UpgradeDataAssets[Index] || !UpgradeDataAssets[Index]->NewTower)
					{
						KRVCHECK(UpgradeDataAssets[Index]);
						UpgradeDataAssets.RemoveAt(Index, 1, false);
					}
					else
					{
						DummyTower->AcquireTowerUpgrade(UpgradeDataAssets[Index]);
					}
				}
				UpgradeDataAssets.Shrink();
				DummyTower->BindToWidgetsBySpawner();
			}

			// Calculate and set waypoint
			PointOnNearestPath = FVector::ZeroVector;
			float MinDistSquaredToPath = TNumericLimits<float>::Max();
			for (TActorIterator<AKRVPath> ActorItr(GetWorld()); ActorItr; ++ActorItr)
			{
				AKRVPath* KRVPath = *ActorItr;
				float ClosestTimeOfPath = KRVPath->FindClosestTimeToWorldLocation(GetActorLocation());
				FVector LocationofClosestTime;
				if (KRVPath->GetPathLocationAtTime(LocationofClosestTime, ClosestTimeOfPath))
				{
					float DistSquaredToCompare = FVector::DistSquared2D(LocationofClosestTime, GetActorLocation());
					if (DistSquaredToCompare < MinDistSquaredToPath)
					{
						PointOnNearestPath = LocationofClosestTime;
					}
				}
			}

			if (PointOnNearestPath == FVector::ZeroVector)
			{
				KRVCHECK(PointOnNearestPath != FVector::ZeroVector);
			}
		}
	}

}

void AKRVBuildingSite::GetPointOnNearestPath(FVector& OutVec) const
{
	OutVec = PointOnNearestPath;

}

void AKRVBuildingSite::InitTowerSet(AKRVTower* NewTower)
{
	NewTower->AttachToComponent(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	NewTower->SetBuildingSite(this);
	NewTower->SetRecruitWaypoint(PointOnNearestPath);
	NewTower->IncreaseSellingCostDelegate.BindUObject(this, &AKRVBuildingSite::IncreaseSellingCost);
	NewTower->OnSellingFinishedDelegate.AddUObject(this, &AKRVBuildingSite::SellTower);
	NewTower->BuildTowerDelegate.BindUObject(this, &AKRVBuildingSite::BuildTower);
	TowerInSite = NewTower;

}

bool AKRVBuildingSite::BuildTower(TSubclassOf<class AKRVTower> NewTowerClass, float BuildTime)
{
	SetActorHiddenInGame(false);
	FlagMeshComponent->SetVisibility(false, true);
	UnderConstructionMeshComponent->SetVisibility(true, true);
	OnBuildStartParticleComponent->ActivateSystem();
	OnBuildStartAudioComponent->Play();
	DummyTower->SetActorEnableCollision(true);

	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		AKRVTower* NewSpawnedTower = GetWorld()->SpawnActor<AKRVTower>
			(NewTowerClass, GetActorTransform(), SpawnParameters);
		KRVCHECK(NewSpawnedTower);
		if (NewSpawnedTower)
		{
			InitTowerSet(NewSpawnedTower);
			NewSpawnedTower->BindToWidgetsBySpawner();
			NewSpawnedTower->PooledComponent->HideNearActor(true);
			GetWorldTimerManager().SetTimer(TowerBuildingTimer, this, &AKRVBuildingSite::OnTowerBuildFinished,
				BuildTime + TowerBuildFinishDelay, false);
			return true;
		}
	}
	return false;

}

void AKRVBuildingSite::OnTowerBuildFinished()
{
	UnderConstructionMeshComponent->SetVisibility(false, true);
	SetActorHiddenInGame(true);
	DummyTower->SetActorEnableCollision(false);

	if (TowerInSite.IsValid())
	{
		AKRVTower* NewTower = TowerInSite.Get();
		NewTower->PooledComponent->HideNearActor(false);
		NewTower->BePossessedByAIController(EKRVTeam::Player);
		NewTower->OnSpawned();
	}

}

void AKRVBuildingSite::SellTower(AKRVTower* TowerToSell)
{
	SetActorHiddenInGame(false);
	FlagMeshComponent->SetVisibility(true, true);
	DummyTower->SetActorEnableCollision(true);

	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		AKRVInGameGameMode* KRVInGameGameMode = Cast<AKRVInGameGameMode>(GetWorld()->GetAuthGameMode());
		KRVCHECK(KRVInGameGameMode);
		if (KRVInGameGameMode)
		{
			KRVInGameGameMode->OnTowerSold(SellingCost * SellingCostScale, TowerToSell);
			SellingCost = 0;
		}
	}

}

int32 AKRVBuildingSite::GetSellingCost() const
{
	return SellingCost * SellingCostScale;

}

void AKRVBuildingSite::IncreaseSellingCost(int32 CostToIncrease)
{
	SellingCost += CostToIncrease;

}

