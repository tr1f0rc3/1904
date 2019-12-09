// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/KRVSkill.h"
#include "KRVItem.generated.h"

DECLARE_DELEGATE_RetVal_TwoParams(int32, FGetRemainingItemStockDelegate, class AKRVItem*, class AKRVCombatUnit*);
DECLARE_DELEGATE_RetVal_TwoParams(int32, FUseItemDelegate, class AKRVItem*, class AKRVCombatUnit*);

UCLASS()
class KRV_API AKRVItem : public AKRVSkill
{
	GENERATED_BODY()
	
public:
	virtual bool LoadFromDataAsset_Implementation() override;
	class UKRVItemDataAsset* GetItemDataAsset() const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		class UKRVItemDataAsset* ItemDataAsset;

public:
	virtual bool IsAbleToCastSkill_Implementation(AKRVCombatUnit* NewCaster = nullptr) override;

protected:
	virtual void OnCastSkill_General(AKRVCombatUnit* NewSkillCaster) override;

public:
	UPROPERTY(BlueprintReadOnly, Category = "Item")
		int32 ItemSlotIndex = -1;

	FGetRemainingItemStockDelegate GetRemainingItemStockDelegate;
	FUseItemDelegate UseItemDelegate;

};
