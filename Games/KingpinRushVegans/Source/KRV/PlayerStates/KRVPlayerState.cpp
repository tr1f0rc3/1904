// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVPlayerState.h"
#include "Kismet/GameplayStatics.h"

#include "SaveGames/KRVInGameSave.h"

AKRVPlayerState::AKRVPlayerState()
{
}

bool AKRVPlayerState::ResetInGameSave()
{
	LoadedInGameSave = NewObject<UKRVInGameSave>();
	LoadedInGameSave->SaveSlotString = SaveSlotString;
	return UGameplayStatics::SaveGameToSlot(LoadedInGameSave, SaveSlotString, 0);

}

bool AKRVPlayerState::LoadInGameSave()
{
	LoadedInGameSave = Cast<UKRVInGameSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotString, 0));
	if (!LoadedInGameSave)
	{
		LoadedInGameSave = GetMutableDefault<UKRVInGameSave>();
	}
	else
	{
		return true;
	}
	return false;

}

bool AKRVPlayerState::SaveInGameSave()
{
	return UGameplayStatics::SaveGameToSlot(GetInGameSave(), SaveSlotString, 0);

}

void AKRVPlayerState::SetSaveSlotString(const FString& NewString)
{
	SaveSlotString = NewString;

}

UKRVInGameSave* AKRVPlayerState::GetInGameSave()
{
	if (!LoadedInGameSave)
	{
		LoadedInGameSave = Cast<UKRVInGameSave>(UGameplayStatics::LoadGameFromSlot(SaveSlotString, 0));
	}
	return LoadedInGameSave;

}




