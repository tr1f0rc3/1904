// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVItem.h"
#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVItemDataAsset.h"
#include "Pawns/KRVCombatUnit.h"
#include "GameModes/KRVInGameGameMode.h"

bool AKRVItem::LoadFromDataAsset_Implementation()
{
	if (Super::LoadFromDataAsset_Implementation())
	{
		ItemDataAsset = Cast<UKRVItemDataAsset>(GetDataAsset_Implementation());

		// Item skill setup
		KRVCHECK(GetWorld());
		if (GetWorld())
		{
			AKRVInGameGameMode* KRVInGameGameMode = UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(GetWorld());
			KRVCHECK(KRVInGameGameMode);
			if (KRVInGameGameMode)
			{
				GetRemainingItemStockDelegate.BindUObject(KRVInGameGameMode, &AKRVInGameGameMode::GetRemainingStock);
				UseItemDelegate.BindUObject(KRVInGameGameMode, &AKRVInGameGameMode::UseItem);
			}
		}
		return true;
	}
	return false;
	
}

UKRVItemDataAsset* AKRVItem::GetItemDataAsset() const
{
	return ItemDataAsset;

}

bool AKRVItem::IsAbleToCastSkill_Implementation(AKRVCombatUnit* NewCaster /*= nullptr*/)
{
	if (Super::IsAbleToCastSkill(NewCaster))
	{
		if (GetRemainingItemStockDelegate.IsBound() &&
			GetRemainingItemStockDelegate.Execute(this, NewCaster))
		{
			return true;
		}
	}
	return false;

}

void AKRVItem::OnCastSkill_General(AKRVCombatUnit* NewSkillCaster)
{
	if (!NewSkillCaster)
	{
		KRVCHECK(NewSkillCaster);
	}
	else
	{
		Super::OnCastSkill_General(NewSkillCaster);

		if (UseItemDelegate.IsBound())
		{
			KRVCHECK(UseItemDelegate.Execute(this, NewSkillCaster));
		}
	}
	
}
