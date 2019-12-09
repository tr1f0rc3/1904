// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVHeroSpawner.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Actors/KRVSkill.h"
#include "Actors/KRVItem.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Pawns/KRVHero.h"
#include "Controllers/KRVInGamePlayerController.h"

#include "PlayerStates/KRVInGamePlayerState.h"
#include "KRVGameInstance.h"

AKRVHeroSpawner::AKRVHeroSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SCENE_COMPONENT"));
	SetRootComponent(SceneComponent);

}

void AKRVHeroSpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();


}

void AKRVHeroSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (GetWorld())
	{
		if (LoadHeroSaveData() && LoadHeroClass())
		{
			KRVCHECK(SpawnHero());
		}
		if (LoadCommanderSaveData())
		{
			KRVCHECK(SpawnCommander());
		}

		// Set delay to wait for loading widget.
		// 위젯이 로딩 되기 전에 바인딩하면 안되므로 바인딩을 지연시킴.
		GetWorldTimerManager().SetTimer(BindToWidgetTimer, this, &AKRVHeroSpawner::BindToWidgets, 0.1f, false);
	}

}

void AKRVHeroSpawner::BindToWidgets()
{
	AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
	if (KRVInGamePlayerController)
	{
		if (SpawnedCommander)
		{
			KRVInGamePlayerController->BindCommanderToWidget(Cast<AKRVCombatUnit>(SpawnedCommander));
		}

		if (SpawnedHero)
		{
			KRVInGamePlayerController->BindHeroToWidget(SpawnedHero);
		}
	}
}

bool AKRVHeroSpawner::SpawnHero()
{
	if (!GetWorld())
	{
		KRVCHECK(GetWorld());
	}
	else
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (HeroClassToSpawn)
		{
			SpawnedHero = GetWorld()->SpawnActor<AKRVHero>(HeroClassToSpawn, GetActorLocation(), GetActorRotation(), SpawnParameters);
			if (!SpawnedHero)
			{
				KRVCHECK(SpawnedHero);
			}
			else
			{
				SpawnedHero->SetSkillLevels(HeroSaveDataToLoad.SkillMasterySaveData);
				SpawnedHero->BePossessedByAIController(EKRVTeam::Player);
				return true;
			}
		}
	}
	return false;

}

bool AKRVHeroSpawner::LoadHeroSaveData()
{
	AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(this);
	if (!KRVInGamePlayerState)
	{
		KRVCHECK(KRVInGamePlayerState);
		return false;
	}
	else
	{
		KRVInGamePlayerState->GetHeroSaveData(HeroSaveDataToLoad);
		return true;
	}

}

bool AKRVHeroSpawner::LoadCommanderSaveData()
{
	AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(this);
	UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(this);
	if (!KRVInGamePlayerState || !KRVGameInstance)
	{
		KRVCHECK(KRVInGamePlayerState);
		KRVCHECK(KRVGameInstance);
		return false;
	}
	else
	{
		// Item Skills;
		TArray<FKRVItemStockData> ItemStockDataArray;
		KRVInGamePlayerState->GetItemStockData(ItemStockDataArray);
		for (const FKRVItemStockData& ItemStockData : ItemStockDataArray)
		{
			TSubclassOf<AKRVItem> ItemClassToAcquire = KRVGameInstance->GetItemClass(ItemStockData.ItemName);
			ItemClassesToAcquire.Emplace(ItemClassToAcquire);
		}
		return true;
	}

}

bool AKRVHeroSpawner::LoadHeroClass()
{
	UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(this);
	if (KRVGameInstance)
	{
		HeroClassToSpawn = KRVGameInstance->GetHeroClass(HeroSaveDataToLoad.HeroName);
		if (!HeroClassToSpawn)
		{
			KRVCHECK(HeroClassToSpawn);
		}
		else
		{
			return true;
		}
	}
	return false;

}

bool AKRVHeroSpawner::SpawnCommander()
{
	if (!GetWorld())
	{
		KRVCHECK(GetWorld());
	}
	else
	{
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

		if (CommanderClassToSpawn)
		{
			SpawnedCommander = GetWorld()->SpawnActor<AKRVCharacter>(CommanderClassToSpawn, GetActorLocation(), GetActorRotation(), SpawnParameters);
			if (!SpawnedCommander)
			{
				KRVCHECK(SpawnedCommander);
			}
			else
			{
				for (int32 Index = 0; Index < ItemClassesToAcquire.Num(); ++Index)
				{
					SpawnedCommander->AcquireSkill(ItemClassesToAcquire[Index]);
				}
				SpawnedCommander->BePossessedByAIController(EKRVTeam::Player);
				SpawnedCommander->SetGlobalCaster(true);
				return true;
			}
		}
	}
	return false;

}

