// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVGameInstance.h"
#include "Engine/DataTable.h"
#include "Engine/ObjectLibrary.h"
#include "Engine/AssetManager.h"
#include "Sound/SoundCue.h"
#include "Particles/ParticleSystem.h"
#include "Engine/StreamableManager.h"

#include "KRV.h"
#include "Structs/KRVCharacterStatData.h"
#include "Structs/KRVBuffStatData.h"
#include "Structs/KRVTowerStatData.h"
#include "DataAssets/KRVCharacterDataAsset.h"
#include "DataAssets/KRVDamageEffectDataAsset.h"
#include "DataAssets/KRVLevelDataAsset.h"
#include "DataAssets/KRVTowerDataAsset.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "DataAssets/KRVDataAsset.h"
#include "DataAssets/KRVHeroDataAsset.h"

#include "Actors/KRVSkill.h"
#include "Actors/KRVItem.h"
#include "Pawns/KRVHero.h"

UKRVGameInstance::UKRVGameInstance()
{
	FString CharacterStatDataPath = TEXT("/Game/Resources/DataTables/DT_CharacterStats.DT_CharacterStats");
	static ConstructorHelpers::FObjectFinder<UDataTable> CharacterStatDTObject(*CharacterStatDataPath);
	KRVCHECK(CharacterStatDTObject.Succeeded());
	CharacterStatDT = CharacterStatDTObject.Object;

	FString BuffStatDataPath = TEXT("/Game/Resources/DataTables/DT_BuffStats.DT_BuffStats");
	static ConstructorHelpers::FObjectFinder<UDataTable> BuffStatDTObject(*BuffStatDataPath);
	KRVCHECK(BuffStatDTObject.Succeeded());
	BuffStatDT = BuffStatDTObject.Object;

	FString TowerStatDataPath = TEXT("/Game/Resources/DataTables/DT_TowerStats.DT_TowerStats");
	static ConstructorHelpers::FObjectFinder<UDataTable> TowerStatDTObject(*TowerStatDataPath);
	KRVCHECK(TowerStatDTObject.Succeeded());
	TowerStatDT = TowerStatDTObject.Object;

}

void UKRVGameInstance::Init()
{
	LoadGlobalEffects();

}

TArray<UObject*> UKRVGameInstance::LoadObjectLibrary(const FString& Path, TSubclassOf<UObject> ObjectClass)
{
	// https://answers.unrealengine.com/questions/421736/object-library-in-blueprints-or-from-c.html
	TArray<UObject*> Assets;
	UObjectLibrary* ObjectLibrary = UObjectLibrary::CreateLibrary(ObjectClass, false, GIsEditor);
	if (ObjectLibrary != nullptr)
	{
		ObjectLibrary->AddToRoot();
		FString NewPath = TEXT("/Game") / Path;
		int32 NumOfAssetDatas = ObjectLibrary->LoadAssetDataFromPath(NewPath);
		TArray<FAssetData> AssetDatas;
		ObjectLibrary->GetAssetDataList(AssetDatas);

		UObject* Asset;
		for (int32 i = 0; i < AssetDatas.Num(); ++i)
		{
			FAssetData& AssetData = AssetDatas[i];
			Asset = AssetData.GetAsset();
			if (Asset)
			{
				Assets.Add(AssetData.GetAsset());
			}
		}
	}
	return Assets;

}

UKRVLevelDataAsset* UKRVGameInstance::GetLevelDataAsset(FString NewLevelString) const
{
	FString MapDANameString = MapDAPrefix + NewLevelString;
	FString MapDAPathString = MapDirString + MapDANameString + FString(TEXT(".")) + MapDANameString;
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous<UKRVLevelDataAsset>(FSoftObjectPath(MapDAPathString), true);

}

FKRVCharacterStatData* UKRVGameInstance::GetCharacterStatData(FName _Name)
{
	FKRVCharacterStatData* LoadedData = nullptr;
	auto SearchResult = CharacterStatDataMap.Find(_Name);
	if (SearchResult)
	{
		bool IsValidIndex = CharacterStatDataArray.IsValidIndex(*SearchResult);
		if (!IsValidIndex)
		{
			KRVCHECK(IsValidIndex);
		}
		else
		{
			LoadedData = &(CharacterStatDataArray[*SearchResult]);
			return LoadedData;
		}
	}

	KRVLOG(Log, TEXT("Start loading %s"), *(_Name.ToString()));
	LoadedData = CharacterStatDT->FindRow<FKRVCharacterStatData>(_Name, TEXT(""));
	if (!LoadedData)
	{
		KRVLOG(Error, TEXT("%s could not found"), *(_Name.ToString()));
	}
	else
	{
		int32 NewIndex = CharacterStatDataArray.Emplace(*LoadedData);
		CharacterStatDataMap.Emplace(_Name, NewIndex);
	}
	return LoadedData;

}

FKRVBuffStatData* UKRVGameInstance::GetBuffStatData(FName _Name)
{
	FKRVBuffStatData* LoadedData = nullptr;
	auto SearchResult = BuffStatDataMap.Find(_Name);
	if (SearchResult)
	{
		bool IsValidIndex = BuffStatDataArray.IsValidIndex(*SearchResult);
		if (!IsValidIndex)
		{
			KRVCHECK(IsValidIndex);
		}
		else
		{
			LoadedData = &(BuffStatDataArray[*SearchResult]);
			return LoadedData;
		}
	}

	KRVLOG(Log, TEXT("Start loading %s"), *(_Name.ToString()));
	LoadedData = BuffStatDT->FindRow<FKRVBuffStatData>(_Name, TEXT(""));
	if (!LoadedData)
	{
		KRVLOG(Error, TEXT("%s could not found"), *(_Name.ToString()));
	}
	else
	{
		int32 NewIndex = BuffStatDataArray.Emplace(*LoadedData);
		BuffStatDataMap.Emplace(_Name, NewIndex);
	}
	return LoadedData;

}

FKRVTowerStatData* UKRVGameInstance::GetTowerStatData(FName _Name)
{
	FKRVTowerStatData* LoadedData = nullptr;
	auto SearchResult = TowerStatDataMap.Find(_Name);
	if (SearchResult)
	{
		bool IsValidIndex = TowerStatDataArray.IsValidIndex(*SearchResult);
		if (!IsValidIndex)
		{
			KRVCHECK(IsValidIndex);
		}
		else
		{
			LoadedData = &(TowerStatDataArray[*SearchResult]);
			return LoadedData;
		}
	}

	KRVLOG(Log, TEXT("Start loading %s"), *(_Name.ToString()));
	LoadedData = TowerStatDT->FindRow<FKRVTowerStatData>(_Name, TEXT(""));
	if (!LoadedData)
	{
		KRVLOG(Error, TEXT("%s could not found"), *(_Name.ToString()));
	}
	else
	{
		int32 NewIndex = TowerStatDataArray.Emplace(*LoadedData);
		TowerStatDataMap.Emplace(_Name, NewIndex);
	}
	return LoadedData;

}

UParticleSystem* UKRVGameInstance::GetDamageEffectTypeParticle(EDamageEffectType _DamageType)
{
	int32* IndexPtr = DamageEffectTypeMap.Find(_DamageType);
	if (IndexPtr)
	{
		return DamageEffectTypeParticle[*IndexPtr];
	}
	else
	{
		KRVCHECK(DamageEffectTypeMap.Contains(_DamageType));
	}
	return nullptr;

}

USoundCue* UKRVGameInstance::GetDamageEffectTypeSound(EDamageEffectType _DamageType)
{
	int32* IndexPtr = DamageEffectTypeMap.Find(_DamageType);
	if (IndexPtr)
	{
		return DamageEffectTypeSound[*IndexPtr];
	}
	else
	{
		KRVCHECK(DamageEffectTypeMap.Contains(_DamageType));
	}
	return nullptr;

}

void UKRVGameInstance::LoadGlobalEffects()
{
	TArray<UObject*> Assets;
	Assets = LoadObjectLibrary(DamageTypeEffectPath, UKRVDamageEffectDataAsset::StaticClass());
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	int32 i = -1;
	for (auto Asset : Assets)
	{
		UKRVDamageEffectDataAsset* DamageEffectDA = Cast<UKRVDamageEffectDataAsset>(Asset);
		if (DamageEffectDA)
		{
			DamageEffectTypeMap.Emplace(DamageEffectDA->DamageEffectType, ++i);
			DamageEffectTypeParticle.Emplace(Streamable.LoadSynchronous(DamageEffectDA->HitParticle, true));
			DamageEffectTypeSound.Emplace(Streamable.LoadSynchronous(DamageEffectDA->HitSound, true));
		}
	}

}

TSubclassOf<class AKRVSkill> UKRVGameInstance::GetSkillClass(const FName& ClassNameToGet) const
{
	FString DataToLoad = SkillClassPrefix + ClassNameToGet.ToString() + FString(TEXT(".")) + SkillClassPrefix + ClassNameToGet.ToString() + FString(TEXT("_C"));
	FSoftClassPath SkillClassPath = FSoftClassPath(SkillClassDir + DataToLoad);
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous(TSoftClassPtr<AKRVSkill>(SkillClassPath), true);

}

class UKRVDataAsset* UKRVGameInstance::GetSkillDA(const FName& ClassNameToGet) const
{
	FString DataToLoad = SkillDAPrefix + ClassNameToGet.ToString() + FString(TEXT(".")) + SkillDAPrefix + ClassNameToGet.ToString() + FString(TEXT("_C"));
	FSoftObjectPath SkillDAPath = FSoftObjectPath(SkillClassDir + DataToLoad);
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous(TSoftObjectPtr<UKRVDataAsset>(SkillDAPath), true);

}

TSubclassOf<class AKRVSkill> UKRVGameInstance::GetItemClass(const FName& ClassNameToGet) const
{
	FString DataToLoad = ItemClassPrefix + ClassNameToGet.ToString() + FString(TEXT(".")) + ItemClassPrefix + ClassNameToGet.ToString() + FString(TEXT("_C"));
	FSoftClassPath ItemClassPath = FSoftClassPath(ItemClassDir + DataToLoad);
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous(TSoftClassPtr<AKRVItem>(ItemClassPath), true);

}

class UKRVItemDataAsset* UKRVGameInstance::GetItemDA(const FName& ClassNameToGet) const
{
	FString DataToLoad = ItemDAPrefix + ClassNameToGet.ToString() + FString(TEXT(".")) + ItemDAPrefix + ClassNameToGet.ToString() + FString(TEXT("_C"));
	FSoftObjectPath ItemDAPath = FSoftObjectPath(ItemClassDir + DataToLoad);
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous(TSoftObjectPtr<UKRVItemDataAsset>(ItemDAPath), true);

}

TSubclassOf<class AKRVHero> UKRVGameInstance::GetHeroClass(const FName& ClassNameToGet) const
{
	FString DataToLoad = HeroClassPrefix + ClassNameToGet.ToString() + FString(TEXT(".")) + HeroClassPrefix + ClassNameToGet.ToString() + FString(TEXT("_C"));
	FSoftClassPath HeroClassPath = FSoftClassPath(HeroClassDir + DataToLoad);
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous(TSoftClassPtr<AKRVHero>(HeroClassPath), true);

}

class UKRVHeroDataAsset* UKRVGameInstance::GetHeroDA(const FName& ClassNameToGet) const
{
	FString DataToLoad = HeroDAPrefix + ClassNameToGet.ToString() + FString(TEXT(".")) + HeroDAPrefix + ClassNameToGet.ToString() + FString(TEXT("_C"));
	FSoftObjectPath HeroDAPath = FSoftObjectPath(HeroClassDir + DataToLoad);
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous(TSoftObjectPtr<UKRVHeroDataAsset>(HeroDAPath), true);

}

class UKRVTowerDataAsset* UKRVGameInstance::GetTowerDA(const FName& ClassNameToGet) const
{
	FString DataToLoad = TowerDAPrefix + ClassNameToGet.ToString() + FString(TEXT(".")) + TowerDAPrefix + ClassNameToGet.ToString() + FString(TEXT("_C"));
	FSoftObjectPath TowerDAPath = FSoftObjectPath(TowerClassDir + DataToLoad);
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	return Streamable.LoadSynchronous(TSoftObjectPtr<UKRVTowerDataAsset>(TowerDAPath), true);

}

