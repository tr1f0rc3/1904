// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerStates/KRVInGamePlayerState.h"

#include "KRV.h"
#include "DataAssets/KRVDataAsset.h"
#include "Actors/KRVSkill.h"
#include "Actors/KRVItem.h"

#include "SaveGames/KRVSaveGame.h"
#include "SaveGames/KRVInGameSave.h"
#include "DataAssets/KRVLevelDataAsset.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"
#include "DataAssets/KRVHeroDataAsset.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "Structs/KRVLevelSaveData.h"
#include "Structs/KRVTowerSaveData.h"

AKRVInGamePlayerState::AKRVInGamePlayerState()
{
}

bool AKRVInGamePlayerState::ResetInGameSave()
{
	if (Super::ResetInGameSave())
	{
		return LoadInGameSave();
	}
	return false;

}

bool AKRVInGamePlayerState::LoadInGameSave()
{
	if (Super::LoadInGameSave() && GetInGameSave())
	{
		HeroSaveData = GetInGameSave()->HeroSaveData;
		ItemStockData = GetInGameSave()->ItemStockData;
		HeroSaveData = GetInGameSave()->HeroSaveData;
		ResourceData = GetInGameSave()->ResourceData;
		PlayerStats = GetInGameSave()->PlayerStats;
		EquippedTowerNames = GetInGameSave()->EquippedTowerNames;
		LevelSaveData = GetInGameSave()->LevelSaveData;
		return true;
	}
	return false;

}

void AKRVInGamePlayerState::GainKillRewards(int32 _Gold, int32 _Exp, int32 _Ruby, int32 _Kills)
{
	AddKills(_Kills);
	EarnGold(_Gold);
	GainExp(_Exp);
	EarnRuby(_Ruby);

}


void AKRVInGamePlayerState::AddKills(int32 KillsToAdd)
{
	PlayerStats.Kills += KillsToAdd;
	SaveDataDuringGame();

}

void AKRVInGamePlayerState::EarnGold(int32 GoldToAdd)
{
	PlayerStats.EarnedGold += GoldToAdd;
	SaveDataDuringGame();

}

void AKRVInGamePlayerState::AddSpentGold(int32 GoldToAdd)
{
	PlayerStats.SpentGold += GoldToAdd;
	SaveDataDuringGame();

}

bool AKRVInGamePlayerState::GetEquippedTowerNames(TArray<FName>& OutNames)
{
	OutNames.Empty();
	if (EquippedTowerNames.Num() > 0)
	{
		OutNames = EquippedTowerNames;
		return true;
	}
	else
	{
		KRVCHECK(EquippedTowerNames.Num() > 0);
		return false;
	}

}

bool AKRVInGamePlayerState::GetItemStockData(TArray<FKRVItemStockData>& OutItemStockData)
{
	OutItemStockData.Empty();
	if (ItemStockData.Num() > 0)
	{
		OutItemStockData = ItemStockData;
		return true;
	}
	else
	{
		KRVCHECK(OutItemStockData.Num() > 0);
		return false;
	}
}

int32 AKRVInGamePlayerState::GetRemainingItemStock(AKRVItem* ItemToGet) const
{
	if (!ItemToGet)
	{
		KRVCHECK(ItemToGet);
	}
	else if (ItemStockData.IsValidIndex(ItemToGet->ItemSlotIndex))
	{
		return ItemStockData[ItemToGet->ItemSlotIndex].Stock;
	}
	else
	{
		const FName& NewSkillName = ItemToGet->GetDataAsset_Implementation()->ClassFName;
		for (int32 Index = 0; Index < ItemStockData.Num(); ++Index)
		{
			const FKRVItemStockData& ItemStockDataToCheck = ItemStockData[Index];
			if (ItemStockDataToCheck.ItemName == NewSkillName)
			{
				ItemToGet->ItemSlotIndex = Index;
				return ItemStockDataToCheck.Stock;
			}
		}
	}
	return -1;

}

int32 AKRVInGamePlayerState::UseItem(AKRVItem* ItemToUse)
{
	if (!ItemToUse)
	{
		KRVCHECK(ItemToUse);
	}
	else if (ItemStockData.IsValidIndex(ItemToUse->ItemSlotIndex))
	{
		int32& ItemStockToUse = ItemStockData[ItemToUse->ItemSlotIndex].Stock;
		if (ItemStockToUse > 0)
		{
			--ItemStockToUse;
			OnItemStockChangedDelegate.Broadcast(ItemStockToUse, ItemToUse);
			return ItemStockToUse;
		}
	}
	else
	{
		const FName& NewSkillName = ItemToUse->GetDataAsset_Implementation()->ClassFName;
		for (int32 Index = 0; Index < ItemStockData.Num(); ++Index)
		{
			FKRVItemStockData& ItemStockDataToCheck = ItemStockData[Index];
			if (ItemStockDataToCheck.ItemName == NewSkillName)
			{
				ItemToUse->ItemSlotIndex = Index;
				int32& ItemStockToUse = ItemStockDataToCheck.Stock;
				if (ItemStockToUse > 0)
				{
					--ItemStockToUse;
					OnItemStockChangedDelegate.Broadcast(ItemStockToUse, ItemToUse);
					return ItemStockToUse;
				}
				else
				{
					return -1;
				}
			}
		}
	}
	return -1;

}

void AKRVInGamePlayerState::EarnRuby(int32 RubyToEarn)
{
	PlayerStats.EarnedRuby += RubyToEarn;
	ResourceData.Ruby += RubyToEarn;
	SaveDataDuringGame();

}

int32 AKRVInGamePlayerState::GetHeroLevel() const
{
	return HeroSaveData.Level;

}

void AKRVInGamePlayerState::GetHeroSaveData(FKRVHeroSaveData& OutHeroSaveData)
{
	OutHeroSaveData = HeroSaveData;

}

void AKRVInGamePlayerState::GainExp(int32 ExpToGain)
{
	PlayerStats.GainedExp += ExpToGain;
	HeroSaveData.CurrentLevelExp += ExpToGain;
	CheckExpReachedAndLevelUp();
	SaveDataDuringGame();
	OnExpChangeDelegate.Broadcast(HeroSaveData.CurrentLevelExp, ExpToLevelUp);

}

void AKRVInGamePlayerState::GetHeroSkillMasteryLevels(TArray<FKRVSkillMasterySaveData>& OutSkillMasterySaveData) const
{
	OutSkillMasterySaveData.Empty();
	OutSkillMasterySaveData.Reserve(HeroSaveData.SkillMasterySaveData.Num());
	for (auto SkillMasterySaveDataToEmplace : HeroSaveData.SkillMasterySaveData)
	{
		OutSkillMasterySaveData.Emplace(SkillMasterySaveDataToEmplace);
	}

}


float AKRVInGamePlayerState::GetCurrentExp() const
{
	return HeroSaveData.CurrentLevelExp;

}

float AKRVInGamePlayerState::GetExpToLevelUp() const
{
	return ExpToLevelUp;

}

void AKRVInGamePlayerState::AddRecruits(int32 _Recruits)
{
	PlayerStats.Recruits += _Recruits;
	SaveDataDuringGame();

}

bool AKRVInGamePlayerState::GetLevelSaveData(FKRVLevelSaveData& OutLevelSaveData)
{
	if (LevelSaveData.EarnedStars == 0)
	{
		return false;
	}
	else
	{
		OutLevelSaveData = LevelSaveData;
		return true;
	}

}

void AKRVInGamePlayerState::SetClearData(int32 NewEarnedStars, float NewClearTime)
{
	// If the level is cleared first time,
	// 처음 이 레벨을 클리어 했을 때,
	if (LevelSaveData.EarnedStars == 0 && NewEarnedStars > 0)
	{
		LevelSaveData.bIsRecentlyCleared = true;
	}

	// Update earned stars.
	LevelSaveData.EarnedStars = FMath::Max3(0, LevelSaveData.EarnedStars, NewEarnedStars);

	// Update clear time.
	if (LevelSaveData.EarnedStars > 0)
	{
		LevelSaveData.BestClearTime = FMath::Min(NewClearTime, LevelSaveData.BestClearTime);
	}

	SaveInGameSave();

}

void AKRVInGamePlayerState::SetExpToLevelUp(int32 NewExp)
{
	ExpToLevelUp = NewExp;
	CheckExpReachedAndLevelUp();
	SaveDataDuringGame();

}

void AKRVInGamePlayerState::CheckExpReachedAndLevelUp()
{
	if (ExpToLevelUp == -1)
	{
		return; // Max level reached
	}
	else if (HeroSaveData.CurrentLevelExp >= ExpToLevelUp)
	{
		HeroSaveData.CurrentLevelExp -= ExpToLevelUp;
		++HeroSaveData.Level;
		HeroSaveData.bIsRecentlyUpdated = true;
		OnLevelUpDelegate.Broadcast(HeroSaveData.Level);
		CheckExpReachedAndLevelUp();
	}

}

bool AKRVInGamePlayerState::SaveDataDuringGame()
{
	if (GetInGameSave())
	{
		GetInGameSave()->SaveSlotString = SaveSlotString;
		GetInGameSave()->HeroSaveData = HeroSaveData;
		GetInGameSave()->ItemStockData = ItemStockData;
		GetInGameSave()->ResourceData = ResourceData;
		GetInGameSave()->PlayerStats = PlayerStats;
		return UGameplayStatics::SaveGameToSlot(GetInGameSave(), SaveSlotString, 0);
	}
	return false;

}

bool AKRVInGamePlayerState::SaveInGameSave()
{
	if (GetInGameSave())
	{
		GetInGameSave()->LevelSaveData = LevelSaveData;
		return SaveDataDuringGame();
	}
	return false;

}

