// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "Engine/StreamableManager.h"
#include "GameplayTagContainer.h"
#include "KRVEnums.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Structs/KRVCharacterStatData.h"
#include "Structs/KRVBuffStatData.h"
#include "Structs/KRVTowerStatData.h"
#include "KRVGameInstance.generated.h"

class UDataTable;
class UParticleSystem;
class USoundCue;

UCLASS()
class KRV_API UKRVGameInstance : public UGameInstance
{
	GENERATED_BODY()

public:
	UKRVGameInstance();

public:
	virtual void Init() override;

public:
	UFUNCTION(BlueprintCallable, Category = "ObjectLibrary")
		TArray<UObject*> LoadObjectLibrary(const FString& Path, TSubclassOf<UObject> ObjectClass);

public: // Level
	class UKRVLevelDataAsset* GetLevelDataAsset(FString NewLevelString) const;

protected:
	FString MapDirString = FString(TEXT("/Game/Resources/Maps/"));
	FString MapDAPrefix = FString(TEXT("DA_L_"));


public: // Stat data
	FKRVCharacterStatData* GetCharacterStatData(FName _Name);
	FKRVBuffStatData* GetBuffStatData(FName _Name);
	FKRVTowerStatData* GetTowerStatData(FName _Name);

protected:
	UPROPERTY()
		UDataTable* CharacterStatDT;

	UPROPERTY(BlueprintReadOnly, Category = "Stat")
		TArray<FKRVCharacterStatData> CharacterStatDataArray;
	TMap<FName, int32> CharacterStatDataMap;

	UPROPERTY()
		UDataTable* BuffStatDT;

	UPROPERTY(BlueprintReadOnly, Category = "Stat")
		TArray<FKRVBuffStatData> BuffStatDataArray;
	TMap<FName, int32> BuffStatDataMap;

	UPROPERTY()
		UDataTable* TowerStatDT;

	UPROPERTY(BlueprintReadOnly, Category = "Stat")
		TArray<FKRVTowerStatData> TowerStatDataArray;
	TMap<FName, int32> TowerStatDataMap;

	FStreamableManager StreamableManager;

public: // Effects
	UFUNCTION(BlueprintCallable, Category = "Hit")
		UParticleSystem* GetDamageEffectTypeParticle(EDamageEffectType _DamageType);

	UFUNCTION(BlueprintCallable, Category = "Hit")
		USoundCue* GetDamageEffectTypeSound(EDamageEffectType _DamageType);

protected:
	void LoadGlobalEffects();

protected:
	UPROPERTY(BlueprintReadOnly, Category = "Hit")
		TArray<UParticleSystem*> DamageEffectTypeParticle;

	UPROPERTY(BlueprintReadOnly, Category = "Hit")
		TArray<USoundCue*> DamageEffectTypeSound;

	TMap<EDamageEffectType, int32> DamageEffectTypeMap;

	FString DamageTypeEffectPath = "/Game/Resources/DamageTypes/DMT_Explosion.DMT_Explosion";

public: // Skill
	TSubclassOf<class AKRVSkill> GetSkillClass(const FName& ClassNameToGet) const;
	class UKRVDataAsset* GetSkillDA(const FName& ClassNameToGet) const;

protected:
	FString SkillClassDir = FString(TEXT("/Game/Resources/Actors/Skills/"));
	FString SkillClassPrefix = FString(TEXT("A_S_"));
	FString SkillDAPrefix = FString(TEXT("DA_S_"));

public: // Item
	TSubclassOf<class AKRVSkill> GetItemClass(const FName& ClassNameToGet) const;
	class UKRVItemDataAsset* GetItemDA(const FName& ClassNameToGet) const;

protected:
	FString ItemClassDir = FString(TEXT("/Game/Resources/Actors/Items/"));
	FString ItemClassPrefix = FString(TEXT("A_I_"));
	FString ItemDAPrefix = FString(TEXT("DA_I_"));

public: // Hero
	TSubclassOf<class AKRVHero> GetHeroClass(const FName& ClassNameToGet) const;
	class UKRVHeroDataAsset* GetHeroDA(const FName& ClassNameToGet) const;

protected:
	FString HeroClassDir = FString(TEXT("/Game/Resources/Pawns/Heroes/"));
	FString HeroClassPrefix = FString(TEXT("HR_"));
	FString HeroDAPrefix = FString(TEXT("DA_HR_"));

public: // Tower
	class UKRVTowerDataAsset* GetTowerDA(const FName& ClassNameToGet) const;

protected:
	FString TowerClassDir = FString(TEXT("/Game/Resources/Pawns/Towers/"));
	FString TowerClassPrefix = FString(TEXT("TW_"));
	FString TowerDAPrefix = FString(TEXT("DA_TW_"));

};
