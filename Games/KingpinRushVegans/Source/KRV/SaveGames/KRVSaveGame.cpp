// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVSaveGame.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"

UKRVSaveGame::UKRVSaveGame()
{
}

bool UKRVSaveGame::SpendRuby(int32 RubyToSpend)
{
	if (ResourceData.Ruby >= RubyToSpend)
	{
		ResourceData.Ruby -= RubyToSpend;
		return true;
	}
	else
	{
		KRVLOG(Warning, TEXT("Not enough ruby (%d / %d)"), RubyToSpend, ResourceData.Ruby);
	}
	return false;
}

bool UKRVSaveGame::ChooseHeroByName(const FName& HeroToChoose)
{
	for (int32 Index = 0; Index < HeroSaveData.Num(); ++Index)
	{
		const FKRVHeroSaveData& HeroDataToCheck = HeroSaveData[Index];
		if (HeroDataToCheck.HeroName == HeroToChoose)
		{
			ChosenHeroIndex = Index;
			ChosenHeroName = HeroToChoose;
			return true;
		}
	}
	return false;

}

bool UKRVSaveGame::IsChosenHero(const FName& HeroToCheck)
{
	return ChosenHeroName == HeroToCheck;

}

bool UKRVSaveGame::GetChosenHeroData(FKRVHeroSaveData& OutHeroSaveData)
{
	if (HeroSaveData.IsValidIndex(ChosenHeroIndex))
	{
		if (HeroSaveData[ChosenHeroIndex].HeroName == ChosenHeroName)
		{
			OutHeroSaveData = HeroSaveData[ChosenHeroIndex];
			return true;
		}
	}

	if (!ChooseHeroByName(ChosenHeroName))
	{
		KRVLOG(Error, TEXT("Hero is not found"));
	}
	else if (HeroSaveData.IsValidIndex(ChosenHeroIndex))
	{
		if (HeroSaveData[ChosenHeroIndex].HeroName == ChosenHeroName)
		{
			OutHeroSaveData = HeroSaveData[ChosenHeroIndex];
			return true;
		}
	}
	return false;

}

bool UKRVSaveGame::GetHeroSaveData(FKRVHeroSaveData& OutHeroSaveData, const FName& HeroToGet)
{
	for (FKRVHeroSaveData& HeroDataToCheck : HeroSaveData)
	{
		if (HeroDataToCheck.HeroName == HeroToGet)
		{
			OutHeroSaveData = HeroDataToCheck;
			return true;
		}
	}
	return false;

}

void UKRVSaveGame::SetHeroSaveData(const FKRVHeroSaveData& NewHeroSaveData)
{
	for (FKRVHeroSaveData& HeroDataToCheck : HeroSaveData)
	{
		if (HeroDataToCheck.HeroName == NewHeroSaveData.HeroName)
		{
			HeroDataToCheck = NewHeroSaveData;
			return;
		}
	}
	HeroSaveData.Emplace(NewHeroSaveData);

}

void UKRVSaveGame::GetEquippedItemStockData(TArray<FKRVItemStockData>& OutItemStockData)
{
	OutItemStockData.Empty();

	for (auto& EquippedItemPair : EquippedItemMap)
	{
		bool bIsEquippedItemCorrect = false;
		const FName& EquippedItemName = EquippedItemPair.Key;
		int32& EquippedItemIndex = EquippedItemPair.Value;

		if (ItemStockData.IsValidIndex(EquippedItemIndex))
		{
			const FKRVItemStockData& ItemStockDataToEmplace = ItemStockData[EquippedItemIndex];
			if (ItemStockDataToEmplace.ItemName == EquippedItemName)
			{
				OutItemStockData.Emplace(ItemStockDataToEmplace);
				bIsEquippedItemCorrect = true;
			}
		}

		if (!bIsEquippedItemCorrect)
		{
			for (int32 ItemStockDataLoopIndex = 0; ItemStockDataLoopIndex < ItemStockData.Num(); ++ItemStockDataLoopIndex)
			{
				const FKRVItemStockData& ItemStockDataToEmplace = ItemStockData[ItemStockDataLoopIndex];
				if (ItemStockDataToEmplace.ItemName == EquippedItemName)
				{
					EquippedItemIndex = ItemStockDataLoopIndex;
					OutItemStockData.Emplace(ItemStockDataToEmplace);
					bIsEquippedItemCorrect = true;
				}
			}
		}

		if (!bIsEquippedItemCorrect)
		{
			KRVLOG(Error, TEXT("Equipped item %s is not found in ItemStockData"));
		}
	}

}

bool UKRVSaveGame::EquipItem(const FName& ItemNameToEquip)
{
	if (EquippedItemMap.Num() >= MaxItemsToEquip)
	{
		KRVLOG(Log, TEXT("Inventory is full."))
	}
	else
	{
		for (int32 ItemStockLoopIndex = 0; ItemStockLoopIndex < ItemStockData.Num(); ++ItemStockLoopIndex)
		{
			const FName& ItemNameToCheck = ItemStockData[ItemStockLoopIndex].ItemName;
			if (ItemNameToCheck == ItemNameToEquip)
			{
				EquippedItemMap.Emplace(ItemNameToEquip, ItemStockLoopIndex);
				return true;
			}
		}
	}
	return false;

}

bool UKRVSaveGame::UnequipItem(const FName& ItemNameToUnequip)
{
	const int32* EquippedItemIndexPtr = EquippedItemMap.Find(ItemNameToUnequip);
	if (EquippedItemIndexPtr)
	{
		EquippedItemMap.Remove(ItemNameToUnequip);
		return true;
	}
	return false;

}

bool UKRVSaveGame::IsEquippedItem(const FName& ItemNameToCheck) const
{
	if (EquippedItemMap.Find(ItemNameToCheck))
	{
		return true;
	}
	return false;

}

void UKRVSaveGame::SetItemStockData(const TArray<FKRVItemStockData>& NewItemStockData)
{
	for (const FKRVItemStockData& NewItemStockDataToCheck : NewItemStockData)
	{
		SetItemStockData(NewItemStockDataToCheck);
	}

}

void UKRVSaveGame::SetItemStockData(const FKRVItemStockData& NewItemStockData)
{
	for (FKRVItemStockData& ItemStockDataToUpdate : ItemStockData)
	{
		if (NewItemStockData.ItemName == ItemStockDataToUpdate.ItemName)
		{
			ItemStockDataToUpdate = NewItemStockData;
			break;
		}
	}
	ItemStockData.Emplace(NewItemStockData);


}

bool UKRVSaveGame::GetItemStockData(FKRVItemStockData& OutItemStockData, const FName& ItemNameToGet)
{
	for (FKRVItemStockData& ItemStockDataToUpdate : ItemStockData)
	{
		if (ItemStockDataToUpdate.ItemName == ItemNameToGet)
		{
			OutItemStockData = ItemStockDataToUpdate;
			return true;
		}
	}
	return false;

}

bool UKRVSaveGame::GetLevelSaveData(FKRVLevelSaveData& OutLevelSaveData, const FString& NewLevelString)
{
	for (const FKRVLevelSaveData& LevelSaveDataToCheck : LevelSaveData)
	{
		if (LevelSaveDataToCheck.LevelString == NewLevelString)
		{
			OutLevelSaveData = LevelSaveDataToCheck;
			return true;
		}
	}
	return false;

}

void UKRVSaveGame::SetLevelSaveData(const FKRVLevelSaveData& NewLevelSaveData)
{
	for (FKRVLevelSaveData& LevelSaveDataToCheck : LevelSaveData)
	{
		if (LevelSaveDataToCheck.LevelString == NewLevelSaveData.LevelString)
		{
			LevelSaveDataToCheck = NewLevelSaveData;
			break;
		}
	}
	LevelSaveData.Emplace(NewLevelSaveData);

}

bool UKRVSaveGame::GetTowerSaveData(FKRVTowerSaveData& OutTowerSaveData, const FName& TowerToGet)
{
	for (FKRVTowerSaveData& TowerDataToCheck : TowerSaveData)
	{
		if (TowerDataToCheck.TowerName == TowerToGet)
		{
			OutTowerSaveData = TowerDataToCheck;
			return true;
		}
	}
	return false;

}

void UKRVSaveGame::SetTowerSaveData(const FKRVTowerSaveData& NewTowerSaveData)
{
	for (FKRVTowerSaveData& TowerSaveDataToCheck : TowerSaveData)
	{
		if (TowerSaveDataToCheck.TowerName == NewTowerSaveData.TowerName)
		{
			TowerSaveDataToCheck = NewTowerSaveData;
			break;
		}
	}
	TowerSaveData.Emplace(NewTowerSaveData);


}

void UKRVSaveGame::GetEquippedTowerNames(TArray<FName>& OutTowerSaveData) const
{
	OutTowerSaveData.Empty();
	for (const auto& EquippedTowerPair : EquippedTowerMap)
	{
		OutTowerSaveData.Emplace(EquippedTowerPair.Key);
	}

}

bool UKRVSaveGame::EquipTower(const FName& TowerToEquip)
{
	if (EquippedTowerMap.Num() >= MaxItemsToEquip)
	{
		KRVLOG(Log, TEXT("Cannot equip tower more."))
	}
	else
	{
		for (int32 TowerStockLoopIndex = 0; TowerStockLoopIndex < TowerSaveData.Num(); ++TowerStockLoopIndex)
		{
			const FName& TowerNameToCheck = TowerSaveData[TowerStockLoopIndex].TowerName;
			if (TowerNameToCheck == TowerToEquip)
			{
				EquippedTowerMap.Emplace(TowerNameToCheck, TowerStockLoopIndex);
				return true;
			}
		}
	}
	return false;

}

bool UKRVSaveGame::UnequipTower(const FName& TowerToUnequip)
{
	const int32* EquippedTowerIndexPtr = EquippedTowerMap.Find(TowerToUnequip);
	if (EquippedTowerIndexPtr)
	{
		EquippedTowerMap.Remove(TowerToUnequip);
		return true;
	}
	return false;

}

bool UKRVSaveGame::IsEquippedTower(const FName& TowerToCheck) const
{
	if (EquippedTowerMap.Find(TowerToCheck))
	{
		return true;
	}
	return false;

}
