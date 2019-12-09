// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVMapPlayerState.h"
#include "Kismet/GameplayStatics.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Structs/KRVLevelSaveData.h"
#include "Structs/KRVTowerSaveData.h"
#include "Structs/KRVHeroSaveData.h"
#include "Structs/KRVItemStockData.h"
#include "Structs/KRVSkillMasterySaveData.h"
#include "DataAssets/KRVLevelDataAsset.h"
#include "DataAssets/KRVTowerUpgradeDataAsset.h"
#include "DataAssets/KRVHeroDataAsset.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "SaveGames/KRVSaveGame.h"
#include "SaveGames/KRVInGameSave.h"
#include "KRVGameInstance.h"
#include "Actors/KRVSkill.h"
#include "GameModes/KRVMapGameMode.h"

AKRVMapPlayerState::AKRVMapPlayerState()
{
}

bool AKRVMapPlayerState::ResetSaveGame()
{
	LoadedSaveGame = NewObject<UKRVSaveGame>();
	LoadedSaveGame->SaveSlotString = SaveSlotString;
	return UGameplayStatics::SaveGameToSlot(LoadedSaveGame, SaveSlotString, 0);

}

bool AKRVMapPlayerState::LoadSaveGame()
{
	LoadedSaveGame = Cast<UKRVSaveGame>(UGameplayStatics::LoadGameFromSlot(SaveSlotString, 0));
	if (LoadedSaveGame)
	{
		return true;
	}
	else
	{
		KRVCHECK(LoadedSaveGame);
		return false;
	}

}

bool AKRVMapPlayerState::SaveSaveGame()
{
	return UGameplayStatics::SaveGameToSlot(GetSaveGame(), SaveSlotString, 0);

}

UKRVSaveGame* AKRVMapPlayerState::GetSaveGame() const
{
	return LoadedSaveGame;

}

bool AKRVMapPlayerState::ApplyInGameSaveToSaveGame()
{
	LoadInGameSave();
	if (GetInGameSave())
	{
		GetInGameSave()->SaveInGameSaveToSaveGame();

		if (!GetInGameSave()->LevelSaveData.bIsRecentlyCleared)
		{
			return true;
		}
		else // Do unlock
		{
			UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(this);
			if (!KRVGameInstance)
			{
				KRVCHECK(KRVGameInstance);
			}
			else
			{
				UKRVLevelDataAsset* LoadedLevelDataAsset = KRVGameInstance->GetLevelDataAsset
				(UGameplayStatics::GetCurrentLevelName(this, true));
				if (!LoadedLevelDataAsset)
				{
					KRVCHECK(LoadedLevelDataAsset);
				}
				else
				{
					return UnlockRewardsAndSaveGame(LoadedLevelDataAsset);
				}
			}
		}
	}
	return false;

}

bool AKRVMapPlayerState::UpdateInGameSaveFromSaveGame(const FString& LevelStringToGet)
{
	LoadInGameSave();
	if (GetInGameSave())
	{
		if (GetInGameSave()->LoadInGameSaveFromSaveGame(SaveSlotString, LevelStringToGet))
		{
			return SaveInGameSave();
		}
	}
	return false;

}

void AKRVMapPlayerState::OnRecentlyUnlockedLevelClicked(const FString& ClickedLevel)
{
	FKRVLevelSaveData LevelSaveDataToUpdate;
	if (!GetSaveGame())
	{
		KRVCHECK(GetSaveGame());
	}
	else
	{
		GetSaveGame()->GetLevelSaveData(LevelSaveDataToUpdate, ClickedLevel);
		if (!LevelSaveDataToUpdate.bIsRecentlyUnlocked)
		{
			KRVCHECK(LevelSaveDataToUpdate.bIsRecentlyUnlocked);
		}
		else
		{
			LevelSaveDataToUpdate.bIsRecentlyUnlocked = false;
		}
		GetSaveGame()->SetLevelSaveData(LevelSaveDataToUpdate);
	}

}

bool AKRVMapPlayerState::UnlockRewardsAndSaveGame(const UKRVLevelDataAsset* NewLevelDA)
{
	if (!NewLevelDA || !GetInGameSave() || !GetSaveGame())
	{
		KRVCHECK(NewLevelDA);
		KRVCHECK(GetInGameSave());
		KRVCHECK(GetSaveGame());
	}
	else
	{
		if (GetInGameSave()->LevelSaveData.bIsRecentlyCleared)
		{
			GetInGameSave()->LevelSaveData.bIsRecentlyCleared = false;

			for (UKRVLevelDataAsset* LevelToUnlock : NewLevelDA->LevelsToUnlockWhenClear)
			{
				FKRVLevelSaveData NewLevelSaveData;
				NewLevelSaveData.LevelString = LevelToUnlock->ClassFName.ToString();
				NewLevelSaveData.bIsRecentlyUnlocked = true;
				NewLevelSaveData.bIsUnlocked = true;
				GetSaveGame()->SetLevelSaveData(NewLevelSaveData);
			}

			for (UKRVTowerUpgradeDataAsset* TowerToUnlock : NewLevelDA->TowersToUnlockWhenClear)
			{
				FKRVTowerSaveData NewTowerSaveData;
				NewTowerSaveData.TowerName = TowerToUnlock->ClassFName;
				NewTowerSaveData.bIsRecentlyUnlocked = true;
				NewTowerSaveData.bIsUnlocked = true;
				GetSaveGame()->SetTowerSaveData(NewTowerSaveData);
			}

			for (UKRVHeroDataAsset* HeroToUnlock : NewLevelDA->HeroesToUnlockWhenClear)
			{
				FKRVHeroSaveData NewHeroSaveData;
				NewHeroSaveData.HeroName = HeroToUnlock->ClassFName;
				NewHeroSaveData.bIsRecentlyUpdated = true;
				NewHeroSaveData.bIsUnlocked = true;
				GetSaveGame()->SetHeroSaveData(NewHeroSaveData);
			}

			TArray<FKRVItemStockData> NewItemStockDataArray;
			for (UKRVItemDataAsset* ItemToUnlock : NewLevelDA->ItemsToUnlockWhenClear)
			{
				FKRVItemStockData NewItemStockData;
				NewItemStockData.ItemName = ItemToUnlock->ClassFName;
				NewItemStockData.bIsRecentlyUnlocked = true;
				NewItemStockData.bIsUnlocked = true;
				NewItemStockDataArray.Emplace(NewItemStockData);
			}
			GetSaveGame()->SetItemStockData(NewItemStockDataArray);
		}

		if (GetWorld() && GetInGameSave()->HeroSaveData.bIsRecentlyUpdated)
		{
			FKRVHeroSaveData NewHeroSaveData = GetInGameSave()->HeroSaveData;
			NewHeroSaveData.SkillPointsGiven =
				UKRVBlueprintFunctionLibrary::GetKRVMapGameMode(GetWorld())->GetAvaialbleSkillPointsFromHeroLevel
				(NewHeroSaveData.Level);
			GetSaveGame()->SetHeroSaveData(NewHeroSaveData);
		}

		return SaveSaveGame();

	}
	return false;

}

void AKRVMapPlayerState::OnRecentlyUpdatedHeroClicked(const FName& ClickedHero)
{
	FKRVHeroSaveData HeroSaveDataToUpdate;
	if (!GetSaveGame())
	{
		KRVCHECK(GetSaveGame());
	}
	else
	{
		GetSaveGame()->GetHeroSaveData(HeroSaveDataToUpdate, ClickedHero);
		if (!HeroSaveDataToUpdate.bIsRecentlyUpdated)
		{
			KRVCHECK(HeroSaveDataToUpdate.bIsRecentlyUpdated);
		}
		else
		{
			HeroSaveDataToUpdate.bIsRecentlyUpdated = false;
		}
		GetSaveGame()->SetHeroSaveData(HeroSaveDataToUpdate);
	}

}

bool AKRVMapPlayerState::PickHero(const FName& HeroNameToPick)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else if (KRVSaveGame->ChooseHeroByName(HeroNameToPick))
	{
		OnHeroPickedDelegate.Broadcast(HeroNameToPick);
		return true;
	}
	return false;

}

bool AKRVMapPlayerState::IsPickedHero(const FName& HeroToCheck)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else
	{
		return KRVSaveGame->IsChosenHero(HeroToCheck);
	}
	return false;


}

bool AKRVMapPlayerState::GivePointToHeroSkill(class UKRVDataAsset* NewDA, const FName& NewHeroName)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame || !NewDA)
	{
		KRVCHECK(KRVSaveGame);
		KRVCHECK(NewDA);
	}
	else
	{
		FKRVHeroSaveData HeroSaveDataToGivePoint;
		if (KRVSaveGame->GetHeroSaveData(HeroSaveDataToGivePoint, NewHeroName))
		{
			if (HeroSaveDataToGivePoint.RemainingSkillPoints <= 0)
			{
				KRVLOG(Log, TEXT("No points remain."));
			}
			else
			{
				FName& SkillNameToGivePoint = NewDA->ClassFName;
				for (FKRVSkillMasterySaveData& SkillMasterySaveDataToCheck : HeroSaveDataToGivePoint.SkillMasterySaveData)
				{
					if (SkillMasterySaveDataToCheck.SkillName == SkillNameToGivePoint)
					{
						if (SkillMasterySaveDataToCheck.MasteryLevel > SkillMasterySaveDataToCheck.MaxMasteryLevel)
						{
							KRVCHECK(SkillMasterySaveDataToCheck.MasteryLevel <= SkillMasterySaveDataToCheck.MaxMasteryLevel);
							break;
						}
						else if (SkillMasterySaveDataToCheck.MasteryLevel == SkillMasterySaveDataToCheck.MaxMasteryLevel)
						{
							KRVLOG(Log, TEXT("The skill mastery level already reached max."));
							break;
						}
						else
						{
							++SkillMasterySaveDataToCheck.MasteryLevel;
							--HeroSaveDataToGivePoint.RemainingSkillPoints;
							KRVSaveGame->SetHeroSaveData(HeroSaveDataToGivePoint);
							OnPointGivenToHeroSkillDelegate.Broadcast(SkillMasterySaveDataToCheck.MasteryLevel, NewDA, NewHeroName);
							OnRemainingSkillPointsChangedDelegate.Broadcast(HeroSaveDataToGivePoint.RemainingSkillPoints, NewHeroName);
							return true;
						}
					}
				}
			}
		}
	}
	return false;

}

bool AKRVMapPlayerState::ResetHeroSkillPoints(const FName& NewHeroName)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else
	{
		FKRVHeroSaveData HeroSaveDataToResetPoints;
		if (KRVSaveGame->GetHeroSaveData(HeroSaveDataToResetPoints, NewHeroName))
		{
			for (FKRVSkillMasterySaveData& SkillMasterySaveDataToResetPoints :
				HeroSaveDataToResetPoints.SkillMasterySaveData)
			{
				SkillMasterySaveDataToResetPoints.MasteryLevel = 0;
			}
			HeroSaveDataToResetPoints.RemainingSkillPoints = HeroSaveDataToResetPoints.SkillPointsGiven;
			KRVSaveGame->SetHeroSaveData(HeroSaveDataToResetPoints);
			OnRemainingSkillPointsChangedDelegate.Broadcast(HeroSaveDataToResetPoints.RemainingSkillPoints, NewHeroName);
			return true;
		}
	}
	return false;

}

int32 AKRVMapPlayerState::GetPointOfHeroSkill(class UKRVDataAsset* NewDA, const FName& NewHeroName)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame || !NewDA)
	{
		KRVCHECK(KRVSaveGame);
		KRVCHECK(NewDA);
	}
	else
	{
		FKRVHeroSaveData HeroSaveDataToGivePoint;
		if (KRVSaveGame->GetHeroSaveData(HeroSaveDataToGivePoint, NewHeroName))
		{
			FName& SkillNameToGetPoint = NewDA->ClassFName;
			for (FKRVSkillMasterySaveData& SkillMasterySaveDataToCheck : HeroSaveDataToGivePoint.SkillMasterySaveData)
			{
				if (SkillMasterySaveDataToCheck.SkillName == SkillNameToGetPoint)
				{
					return SkillMasterySaveDataToCheck.MasteryLevel;
				}
			}
		}
	}
	return -1;

}

void AKRVMapPlayerState::OnRecentlyUnlockedItemClicked(const FName& ClickedItem)
{
	FKRVItemStockData ItemStockDataToUpdate;
	if (!GetSaveGame())
	{
		KRVCHECK(GetSaveGame());
	}
	else
	{
		GetSaveGame()->GetItemStockData(ItemStockDataToUpdate, ClickedItem);
		if (!ItemStockDataToUpdate.bIsRecentlyUnlocked)
		{
			KRVCHECK(ItemStockDataToUpdate.bIsRecentlyUnlocked);
		}
		else
		{
			ItemStockDataToUpdate.bIsRecentlyUnlocked = false;
		}
		GetSaveGame()->SetItemStockData(ItemStockDataToUpdate);
	}

}

bool AKRVMapPlayerState::EquipItem(const FName& ItemToEquip)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else if (KRVSaveGame->EquipItem(ItemToEquip))
	{
		OnItemEquippedDelegate.Broadcast(ItemToEquip);
		return true;
	}
	return false;

}

bool AKRVMapPlayerState::UnequipItem(const FName& ItemToUnequip)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else if (KRVSaveGame->UnequipItem(ItemToUnequip))
	{
		OnItemUnequippedDelegate.Broadcast(ItemToUnequip);
		return true;
	}
	return false;

}

bool AKRVMapPlayerState::IsEquippedItem(const FName& ItemToCheck) const
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else
	{
		return KRVSaveGame->IsEquippedItem(ItemToCheck);
	}
	return false;

}

bool AKRVMapPlayerState::BuyItem(const UKRVItemDataAsset* ItemDAToBuy, int32 AmountToBuy /*= 1*/)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame || !ItemDAToBuy || AmountToBuy > 0)
	{
		KRVCHECK(KRVSaveGame);
		KRVCHECK(ItemDAToBuy);
		KRVCHECK(AmountToBuy > 0);
	}
	else
	{
		const FName& ItemNameToBuy = ItemDAToBuy->ClassFName;

		TArray<FKRVItemStockData> ItemStockDataArrayToBuy;
		ItemStockDataArrayToBuy.Init(FKRVItemStockData(), 1);
		FKRVItemStockData& ItemStockDataToBuy = ItemStockDataArrayToBuy[0];

		if (!KRVSaveGame->GetItemStockData(ItemStockDataToBuy, ItemNameToBuy))
		{
			KRVLOG(Error, TEXT("Item is not found"));
		}
		else if (!ItemStockDataToBuy.bIsUnlocked)
		{
			KRVLOG(Warning, TEXT("Item is locked"));
		}
		else if (ItemDAToBuy->Price >= 0 &&
			KRVSaveGame->SpendRuby(AmountToBuy * ItemDAToBuy->Price))
		{
			ItemStockDataToBuy.Stock += AmountToBuy;
			KRVSaveGame->SetItemStockData(ItemStockDataArrayToBuy);
			OnItemPurchasedDelegate.Broadcast(ItemStockDataToBuy.Stock, ItemStockDataToBuy.ItemName);
			OnRubyChangedDelegate.Broadcast(KRVSaveGame->ResourceData.Ruby);
			return true;
		}
	}
	return false;

}

void AKRVMapPlayerState::OnRecentlyUnlockedTowerClicked(const FName& ClickedTower)
{
	FKRVTowerSaveData TowerSaveDataToUpdate;
	if (!GetSaveGame())
	{
		KRVCHECK(GetSaveGame());
	}
	else
	{
		GetSaveGame()->GetTowerSaveData(TowerSaveDataToUpdate, ClickedTower);
		if (!TowerSaveDataToUpdate.bIsRecentlyUnlocked)
		{
			KRVCHECK(TowerSaveDataToUpdate.bIsRecentlyUnlocked);
		}
		else
		{
			TowerSaveDataToUpdate.bIsRecentlyUnlocked = false;
		}
		GetSaveGame()->SetTowerSaveData(TowerSaveDataToUpdate);
	}

}

bool AKRVMapPlayerState::EquipTower(const FName& TowerToEquip)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else if (KRVSaveGame->EquipTower(TowerToEquip))
	{
		OnTowerEquippedDelegate.Broadcast(TowerToEquip);
		return true;
	}
	return false;

}

bool AKRVMapPlayerState::UnequipTower(const FName& TowerToUnequip)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else if (KRVSaveGame->UnequipTower(TowerToUnequip))
	{
		OnTowerUnequippedDelegate.Broadcast(TowerToUnequip);
		return true;
	}
	return false;

}

bool AKRVMapPlayerState::IsEquippedTower(const FName& TowerToCheck)
{
	UKRVSaveGame* KRVSaveGame = GetSaveGame();
	if (!KRVSaveGame)
	{
		KRVCHECK(KRVSaveGame);
	}
	else
	{
		return KRVSaveGame->IsEquippedTower(TowerToCheck);
	}
	return false;

}
