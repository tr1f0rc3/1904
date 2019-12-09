// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVGateway.h"
#include "EngineUtils.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Components/BoxComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"
#include "Components/AudioComponent.h"
#include "WidgetComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVDataAsset.h"
#include "DataAssets/KRVGatewayDataAsset.h"
#include "DataAssets/KRVCharacterDataAsset.h"

#include "SceneComponents/SpawnerComponent.h"
#include "SceneComponents/PooledComponent.h"
#include "Actors/KRVPath.h"
#include "Pawns/KRVCharacter.h"

#include "GameModes/KRVInGameGameMode.h"
#include "UIs/KRVIncomingUserWidget.h"
#include "UIs/KRVIconUserWidget.h"

AKRVGateway::AKRVGateway()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX_COMPONENT"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetCollisionProfileName("BlockAllDynamic");
	BoxComponent->InitBoxExtent(FVector(150.f, 150.f, 10.f));

	IncomingParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("INCOMING_PARTICLE_COMPONENT"));
	IncomingParticleComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> INCOMING_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Combat_Lightning/P_DOT_Lightning_01.P_DOT_Lightning_01"));
	if (INCOMING_PARTICLE_OBJ.Succeeded())
	{
		IncomingParticleComponent->SetTemplate(INCOMING_PARTICLE_OBJ.Object);
		IncomingParticleComponent->SetAutoActivate(false);
	}

	NextWaveCallAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("NEXT_WAVE_CALL_AUDIO_COMPONENT"));
	NextWaveCallAudioComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<USoundCue> NEXT_WAVE_CALL_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Alarms_Sirens/Cues/retro_alarm_siren_loop_04_Cue.retro_alarm_siren_loop_04_Cue"));
	if (NEXT_WAVE_CALL_SOUND_OBJ.Succeeded())
	{
		NextWaveCallAudioComponent->SetSound(NEXT_WAVE_CALL_SOUND_OBJ.Object);
		NextWaveCallAudioComponent->SetAutoActivate(false);
	}

	SpawnParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("SPAWN_PARTICLE_COMPONENT"));
	SpawnParticleComponent->SetupAttachment(RootComponent);
	static ConstructorHelpers::FObjectFinder<UParticleSystem> SPAWN_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Ability/Defense/P_Shield_Spawn.P_Shield_Spawn"));
	if (SPAWN_PARTICLE_OBJ.Succeeded())
	{
		SpawnParticleComponent->SetTemplate(SPAWN_PARTICLE_OBJ.Object);
		SpawnParticleComponent->SetAutoActivate(false);
	}

	IncomingWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("INCOMING_WIDGET"));
	IncomingWidget->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FClassFinder<UKRVIconUserWidget> INCOMING_WIDGET_C(TEXT("/Game/Resources/UI/UI_IncomingButton.UI_IncomingButton_C"));
	if (INCOMING_WIDGET_C.Succeeded())
	{
		IncomingWidget->AddRelativeLocation(FVector(200.f, 0.f, 100.f));
		IncomingWidget->SetWidgetSpace(EWidgetSpace::Screen);
		IncomingWidget->SetWidgetClass(INCOMING_WIDGET_C.Class);
		IncomingWidget->SetDrawSize(FVector2D(12.f, 12.f));
		IncomingWidget->SetHiddenInGame(true);
	}

	Pathways.Init(nullptr, MaxPaths);

}

void AKRVGateway::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		for (TActorIterator<AKRVPath> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AKRVPath* KRVPath = *ActorItr;
			if (KRVPath)
			{
				int32 PathID = KRVPath->GetPathID();
				if (!Pathways.IsValidIndex(PathID))
				{
					KRVCHECK(Pathways.IsValidIndex(PathID));
				}
				else
				{
					Pathways[PathID] = KRVPath;
				}
			}
		}
	}

	if (BindToWidget())
	{
		IncomingWidget->SetHiddenInGame(false);
	}

}

void AKRVGateway::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (GetWorld())
	{
		AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
		if (KRVInGameGameMode)
		{
			KRVInGameGameMode->OnNextWavePathsShownDelegate.AddUObject(this, &AKRVGateway::ShowNextWavePaths);
			KRVInGameGameMode->OnNextWaveIncomingDelegate.AddDynamic(this, &AKRVGateway::OnNextWaveIncoming);
			KRVInGameGameMode->OnNextWaveCalledDeleagte.AddDynamic(this, &AKRVGateway::OnNextWaveCalled);
			KRVInGameGameMode->OnGameEndDelegate.AddUObject(this, &AKRVGateway::OnGameEnd);

			RequestNextWaveCallDelegate.AddUObject(KRVInGameGameMode, &AKRVInGameGameMode::CallNextWave);
			RequestToShowNextWavePathsDelegate.BindUObject(KRVInGameGameMode, &AKRVInGameGameMode::ShowNextWavePaths);
			OnCharacterSpawned.AddUObject(KRVInGameGameMode, &AKRVInGameGameMode::OnEnemySpawned);
			OnSpawnInTheWaveFinishedDelegate.BindUObject(KRVInGameGameMode, &AKRVInGameGameMode::OnTheGateFinishSpawnInTheWave);
			OnAllSpawnFinishedDelegate.BindUObject(KRVInGameGameMode, &AKRVInGameGameMode::OnTheGateFinishAllSpawn);
		}
	}

	LoadFromDataAsset_Implementation();

}


UKRVDataAsset* AKRVGateway::GetDataAsset_Implementation() const
{
	return Cast<UKRVDataAsset>(HardPtr_GateWayDataAsset);

}

class UKRVGatewayDataAsset* AKRVGateway::GetGatewayDataAsset() const
{
	return HardPtr_GateWayDataAsset;

}

bool AKRVGateway::LoadFromDataAsset_Implementation()
{
	if (!GetGatewayDataAsset())
	{
		FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
		HardPtr_GateWayDataAsset = Cast<UKRVGatewayDataAsset>
			(Streamable.LoadSynchronous(SoftPtr_GateWayDataAsset.ToSoftObjectPath(), true));
	}
	if (!GetGatewayDataAsset())
	{
		KRVCHECK(GetGatewayDataAsset());
		return false;
	}
	else
	{
		// Get Max Size of pool of each spawner component;
		// 각 스포너컴퍼넌트의 최대 풀 사이즈를 구함;
		TArray<int32> MaxCharactersInPools;
		{
			TArray<int32> CharactersInAWave;
			int32 SCMapIndex = -1;
			for (const auto& CSStruct : GetGatewayDataAsset()->CharacterSpawnData)
			{
				int32* PtrIndex = SpawnerComponentMap.Find(CSStruct.CharacterClass);
				if (PtrIndex)
				{
					CharactersInAWave[*PtrIndex] += CSStruct.CharactersToSpawn;
				}
				else
				{
					CharactersInAWave.Emplace(CSStruct.CharactersToSpawn);
					MaxCharactersInPools.Emplace(0);
					SpawnerComponentMap.Emplace(CSStruct.CharacterClass, ++SCMapIndex);
				}

				if (CSStruct.TimeToNextSpawn < 0.f)
				{
					KRVCHECK(CharactersInAWave.Num() == MaxCharactersInPools.Num());
					for (int32 Index = 0; Index < MaxCharactersInPools.Num(); ++Index)
					{
						MaxCharactersInPools[Index] =
							FMath::Max3(0, MaxCharactersInPools[Index], CharactersInAWave[Index]);
						KRVCHECK(MaxCharactersInPools[Index] > 0);
						CharactersInAWave[Index] = 0;
					}
				}
			}
		}
		{ // Create Spawner Component
			FKRVCharacterSpawnData TemporaryCSStruct;
			for (const auto& SCMap : SpawnerComponentMap)
			{
				TemporaryCSStruct.CharacterClass = SCMap.Key;
				KRVCHECK(SCMap.Value < MaxCharactersInPools.Num());
				TemporaryCSStruct.CharactersToSpawn = 2 * MaxCharactersInPools[SCMap.Value];

				USpawnerComponent* SpawnerComponent = NewObject<USpawnerComponent>(this);
				SpawnerComponent->RegisterComponent();
				SpawnerComponent->AttachToComponent
				(RootComponent, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				SpawnerComponent->SetPoolWithCharacterSpawnData(TemporaryCSStruct);
				SpawnerComponents.Emplace(SpawnerComponent);
			}
		}

		// Set TotalWaves
		for (const auto& CSStruct : GetGatewayDataAsset()->CharacterSpawnData)
		{
			if (CSStruct.TimeToNextSpawn < 0.f)
			{
				++TotalWaves;
			}
		}

		// Add This To GameMode;
		if (GetWorld())
		{
			AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
			if (KRVInGameGameMode)
			{
				KRVInGameGameMode->AddGate(this);
			}
		}
		return true;
	}
	return false;
}

void AKRVGateway::SpawnCharacter(const FKRVCharacterSpawnData& CharacterSpawnData)
{

	int32* PtrSCIndex = SpawnerComponentMap.Find(CharacterSpawnData.CharacterClass);
	if (PtrSCIndex)
	{
		TArray<AActor*> CharacterToBeSpawnedSoon;
		SpawnerComponents[*PtrSCIndex]->SetSpawnRate
		(CharacterSpawnData.CharactersToSpawn, CharacterSpawnData.SpawnRate);
		SpawnerComponents[*PtrSCIndex]->SpawnWithOutArray(CharacterToBeSpawnedSoon);
		for (AActor* CharacterActor : CharacterToBeSpawnedSoon)
		{
			AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(CharacterActor);
			KRVCHECK(KRVCharacter);
			if (KRVCharacter)
			{
				KRVCharacter->PathToFollow = GetPath(CharacterSpawnData.PathID);
				KRVCharacter->BePossessedByAIController(EKRVTeam::Enemy, true);
				OnCharacterSpawned.Broadcast(KRVCharacter);
			}
		}

		if (SpawnParticleComponent)
		{
			SpawnParticleComponent->ActivateSystem();
		}
	}
	else
	{
		KRVLOG(Log, TEXT("Character class not found in array. Spawn Skipped"));
	}

	if (CharacterSpawnData.TimeToNextSpawn < 0.f)
	{
		OnSpawnInTheWaveFinishedDelegate.ExecuteIfBound(CharacterSpawnData.TimeToNextWave, this);
		if (CurrentSpawnIndex >= GetGatewayDataAsset()->CharacterSpawnData.Num() - 1)
		{
			OnAllSpawnFinishedDelegate.ExecuteIfBound(this);
		}
	}
	else if (GetWorld())
	{
		GetWorldTimerManager().SetTimer
		(CharacterSpawnTimer, this, &AKRVGateway::SpawnRecursive, CharacterSpawnData.TimeToNextSpawn, false);
	}

}


class AKRVPath* AKRVGateway::GetPath(int32 Index)
{
	if (!Pathways.IsValidIndex(Index))
	{
		KRVCHECK(Pathways.IsValidIndex(Index));
		return nullptr;
	}
	else if (!Pathways[Index].IsValid())
	{
		KRVCHECK(Pathways[Index].IsValid());
		return nullptr;
	}
	else
	{
		return Pathways[Index].Get();
	}

}

void AKRVGateway::OnGameEnd(EKRVTeam Winner)
{
	for (USpawnerComponent* SpawnerComponent : SpawnerComponents)
	{
		SpawnerComponent->StopSpawning();
	}

}

bool AKRVGateway::BindToWidget()
{
	if (!IncomingWidget)
	{
		KRVCHECK(IncomingWidget);
	}
	else
	{
		UKRVIconUserWidget* CastedIncomingWidget = Cast<UKRVIconUserWidget>(IncomingWidget->GetUserWidgetObject());
		if (!CastedIncomingWidget)
		{
			KRVCHECK(CastedIncomingWidget);
		}
		else
		{
			return CastedIncomingWidget->BindToGateway(this);
		}
	}
	return false;

}

void AKRVGateway::RequestToShowNextWavePaths(bool bIsTrue)
{
	RequestToShowNextWavePathsDelegate.ExecuteIfBound(bIsTrue);

}

bool AKRVGateway::GetNextWaveTooltipText(FText& OutText)
{
	if (GetGatewayDataAsset() &&
		GetGatewayDataAsset()->CharacterSpawnData.IsValidIndex(CurrentSpawnIndex))
	{
		bool bDoesGetTooltip = true;

		FString OutString;
		const TArray<FKRVCharacterSpawnData>& CharacterSpawnDataArrToCheck =
			GetGatewayDataAsset()->CharacterSpawnData;
		for (int32 SpawnIndexToCheck = CurrentSpawnIndex;
			CharacterSpawnDataArrToCheck.IsValidIndex(SpawnIndexToCheck);
			++SpawnIndexToCheck)
		{
			const FKRVCharacterSpawnData& CharacterSpawnDataToCheck = CharacterSpawnDataArrToCheck[SpawnIndexToCheck];

			bDoesGetTooltip &= GetNextWaveTooltip_Internal(OutString, CharacterSpawnDataToCheck);
			if (CharacterSpawnDataToCheck.TimeToNextSpawn < 0.f)
			{
				break;
			}
		}
		OutText = FText::FromString(OutString);
		return bDoesGetTooltip;
	}
	return false;

}

bool AKRVGateway::IsFlyingUnitIncomingInNextWave() const
{
	return bIsFlyingUnitIncomingInNextWave;

}

bool AKRVGateway::GetNextWaveTooltip_Internal(FString& OutString, const FKRVCharacterSpawnData& CharacterSpawnDataToGet)
{
	int32* PtrIndex = SpawnerComponentMap.Find(CharacterSpawnDataToGet.CharacterClass);
	if (PtrIndex)
	{
		AKRVCharacter* CharacterToGet = Cast<AKRVCharacter>(SpawnerComponents[*PtrIndex]->GetFirstSample());
		if (!CharacterToGet)
		{
			KRVCHECK(CharacterToGet);
		}
		else
		{
			UKRVCharacterDataAsset* CharacterDAToGet = CharacterToGet->GetCharacterDataAsset();
			if (!CharacterToGet)
			{
				KRVCHECK(CharacterDAToGet);
			}
			else
			{
				bIsFlyingUnitIncomingInNextWave = false;
				bIsFlyingUnitIncomingInNextWave |= CharacterDAToGet->bIsFlying;

				OutString += (CharacterToGet->GetDataAsset_Implementation()->ClassFText).ToString();
				OutString += FString(TEXT(": "));
				OutString += FString::FromInt(CharacterSpawnDataToGet.CharactersToSpawn);
				OutString += FString(LINE_TERMINATOR);
				return true;
			}
		}
	}
	return false;

}

void AKRVGateway::ShowNextWavePaths(bool bIstrue)
{
	if (!GetGatewayDataAsset())
	{
		KRVCHECK(GetGatewayDataAsset());
	}
	else if (bIsPathsShown != bIstrue)
	{
		const FKRVCharacterSpawnData& CurrentCharacterSpawnData = GetGatewayDataAsset()->CharacterSpawnData[CurrentSpawnIndex];
		if (CurrentCharacterSpawnData.CharacterClass)
		{
			if (!Pathways.IsValidIndex(CurrentCharacterSpawnData.PathID))
			{
				KRVCHECK(Pathways.IsValidIndex(CurrentCharacterSpawnData.PathID));
			}
			else
			{
				Pathways[CurrentCharacterSpawnData.PathID].Get()->ShowIndicators(bIstrue);
				bIsPathsShown = bIstrue;
			}
		}
	}

}

void AKRVGateway::OnNextWaveIncoming_Implementation(float CallTimeLimit /*= -1.f*/)
{
	if (!GetGatewayDataAsset())
	{
		KRVCHECK(GetGatewayDataAsset());
	}
	else
	{
		const FKRVCharacterSpawnData& CurrentCharacterSpawnData = GetGatewayDataAsset()->CharacterSpawnData[CurrentSpawnIndex];
		if (CurrentCharacterSpawnData.CharacterClass)
		{
			// reset wave variables
			bIsFlyingUnitIncomingInNextWave = false;

			// turn on incoming particle
			if (IncomingParticleComponent)
			{
				IncomingParticleComponent->ActivateSystem();
			}

			OnNextWaveIncomingWidgetDelegate.ExecuteIfBound(CallTimeLimit);
		}
	}

}

void AKRVGateway::OnNextWaveCalled_Implementation()
{
	if (IncomingParticleComponent)
	{
		IncomingParticleComponent->DeactivateSystem();
	}

	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer
		(CharacterSpawnTimer, this, &AKRVGateway::SpawnRecursive, DelayToBeginSpawn, false);
	}

	OnNextWaveCalledWidgetDelegate.ExecuteIfBound();

}

int32 AKRVGateway::GetTotalWaves() const
{
	if (!GetGatewayDataAsset())
	{
		KRVCHECK(GetGatewayDataAsset());
		return -1;
	}
	else
	{
		return TotalWaves;
	}

}

void AKRVGateway::RequestNextWaveCall()
{
	RequestNextWaveCallDelegate.Broadcast();

}

void AKRVGateway::SpawnRecursive()
{
	if (GetGatewayDataAsset() &&
		GetGatewayDataAsset()->CharacterSpawnData.IsValidIndex(CurrentSpawnIndex))
	{
		SpawnCharacter(GetGatewayDataAsset()->CharacterSpawnData[CurrentSpawnIndex++]);
	}
	else
	{
		OnAllSpawnFinishedDelegate.ExecuteIfBound(this);
	}

}

