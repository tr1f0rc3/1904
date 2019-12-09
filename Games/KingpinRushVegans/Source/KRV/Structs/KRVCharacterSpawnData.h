// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "KRVCharacterSpawnData.generated.h"

USTRUCT(BlueprintType)
struct FKRVCharacterSpawnData : public FTableRowBase
{
	GENERATED_BODY()

public: // Info for skill, tower(player) and gateway(enemy)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
		TSubclassOf<class AKRVCharacter> CharacterClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
		int32 CharactersToSpawn = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Character")
		float SpawnRate = 0.3f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Wave")
		int32 PathID;

	// -1 means end of spawning in the wave
	// -1 은 해당 웨이브의 마지막을 의미함
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
		float TimeToNextSpawn = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Time")
		float TimeToNextWave = 3.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Resource")
		int32 Ruby = 0.f;
	

};
