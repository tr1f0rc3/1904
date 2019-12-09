// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "KRVEnums.h"
#include "Structs/KRVHeroSaveData.h"
#include "KRVHeroSpawner.generated.h"

UCLASS()
class KRV_API AKRVHeroSpawner : public AActor
{
	GENERATED_BODY()

public:
	AKRVHeroSpawner();

protected:
	virtual void PostInitializeComponents() override;

protected:
	virtual void BeginPlay() override;

protected:
	UFUNCTION()
		void BindToWidgets();

	bool SpawnHero();
	bool LoadHeroSaveData();
	bool LoadHeroClass();

	bool SpawnCommander();
	bool LoadCommanderSaveData();

protected:
	UPROPERTY()
		USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<class AKRVHero> HeroClassToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		FKRVHeroSaveData HeroSaveDataToLoad;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVHero* SpawnedHero;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<class AKRVCharacter> CommanderClassToSpawn;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVCombatUnit* SpawnedCommander;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		TArray<TSubclassOf<class AKRVItem>> ItemClassesToAcquire;


protected:
	FTimerHandle BindToWidgetTimer;

};
