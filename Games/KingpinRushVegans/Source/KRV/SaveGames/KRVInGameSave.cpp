// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVInGameSave.h"
#include "Kismet/GameplayStatics.h"

#include "KRV.h"
#include "KRVSaveGame.h"

bool UKRVInGameSave::LoadInGameSaveFromSaveGame(const FString& NewSaveSlotString, const FString& LevelString)
{
	UKRVSaveGame* LoadedSaveGame = Cast<UKRVSaveGame>(UGameplayStatics::LoadGameFromSlot(NewSaveSlotString, 0));
	if (!LoadedSaveGame)
	{
		KRVCHECK(LoadedSaveGame);
	}
	else
	{
		SaveSlotString = NewSaveSlotString;
		KRVCHECK(LoadedSaveGame->GetChosenHeroData(HeroSaveData));
		LoadedSaveGame->GetEquippedItemStockData(ItemStockData);
		KRVCHECK(LoadedSaveGame->GetLevelSaveData(LevelSaveData, LevelString));
		ResourceData = LoadedSaveGame->ResourceData;
		PlayerStats = LoadedSaveGame->PlayerStats;
		LoadedSaveGame->GetEquippedTowerNames(EquippedTowerNames);
		return true;
	}
	return false;

}

bool UKRVInGameSave::SaveInGameSaveToSaveGame()
{
	UKRVSaveGame* LoadedSaveGame = Cast<UKRVSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotString, 0));
	if (!LoadedSaveGame)
	{
		KRVCHECK(LoadedSaveGame);
	}
	else
	{
		LoadedSaveGame->SaveSlotString = SaveSlotString;
		LoadedSaveGame->SetHeroSaveData(HeroSaveData);
		LoadedSaveGame->SetItemStockData(ItemStockData);
		LoadedSaveGame->ResourceData = ResourceData;
		LoadedSaveGame->PlayerStats = PlayerStats;
		LoadedSaveGame->SetLevelSaveData(LevelSaveData);
		return UGameplayStatics::SaveGameToSlot(LoadedSaveGame, SaveSlotString, 0);
	}
	return false;

}
