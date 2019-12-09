// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVInGameCommanderUserWidget.h"
#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Pawns/KRVCombatUnit.h"
#include "Actors/KRVSkill.h"
#include "Actors/KRVItem.h"
#include "KRVIconUserWidget.h"

void UKRVInGameCommanderUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SkillWidget_1 = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_SkillIcon_1")));
	SkillWidget_2 = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_SkillIcon_2")));
	SkillWidget_3 = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_SkillIcon_3")));

}

bool UKRVInGameCommanderUserWidget::BindToGlobalSkills(class AKRVCombatUnit* NewCommander)
{
	if (!NewCommander)
	{
		KRVCHECK(NewCommander);
	}
	else
	{
		BoundCommander = NewCommander;

		bool bBindResult = true;

		if (!SkillWidget_1 || !SkillWidget_2)
		{
			bBindResult = false;
		}

		for (int32 SkillIndex = 0, SlotIndex = 0; SlotIndex < MaxSkillSlotsToActive; ++SkillIndex)
		{
			FGameplayTag SkillTagToBind;
			if (!NewCommander->GetSkillTagByIndex(SkillTagToBind, SkillIndex))
			{
				break;
			}
			else
			{
				AKRVSkill* SkillToBind = NewCommander->GetSkillByTag(SkillTagToBind);
				AKRVItem* ItemToBind = Cast<AKRVItem>(SkillToBind);

				if (!SkillToBind ||
					SkillToBind->IsAutoCastable() ||
					SkillToBind->GetTargetingType() == ETargetingType::Passive ||
					ItemToBind)
				{
					KRVCHECK(SkillToBind);
					continue;
				}
				else if (SlotIndex == 0 && SkillWidget_1)
				{
					bBindResult &= SkillWidget_1->BindToSkill(SkillTagToBind, NewCommander);
					++SlotIndex;
				}
				else if (SlotIndex == 1 && SkillWidget_2)
				{
					bBindResult &= SkillWidget_2->BindToSkill(SkillTagToBind, NewCommander);
					++SlotIndex;
				}
				else
				{
					break;
				}
			}
		}
		return bBindResult;
	}
	return false;

}

bool UKRVInGameCommanderUserWidget::BindToItems(class AKRVCombatUnit* NewCommander)
{
	if (!NewCommander)
	{
		KRVCHECK(NewCommander);
	}
	else
	{
		BoundCommander = NewCommander;

		bool bBindResult = true;
		if (!SkillWidget_1 || !SkillWidget_2 || !SkillWidget_3)
		{
			bBindResult = false;
		}

		for (int32 SkillIndex = 0, SlotIndex = 0; SlotIndex < MaxItemsToEquip; ++SkillIndex)
		{
			FGameplayTag SkillTagToBind;
			if (!NewCommander->GetSkillTagByIndex(SkillTagToBind, SkillIndex))
			{
				break;
			}
			else
			{
				AKRVItem* ItemToBind = Cast<AKRVItem>(NewCommander->GetSkillByTag(SkillTagToBind));
				if (!ItemToBind || ItemToBind->IsAutoCastable())
				{
					continue;
				}
				else if (SlotIndex == 0 && SkillWidget_1)
				{
					bBindResult &= SkillWidget_1->BindToItem(SkillTagToBind, NewCommander);
					++SlotIndex;
				}
				else if (SlotIndex == 1 && SkillWidget_2)
				{
					bBindResult &= SkillWidget_2->BindToItem(SkillTagToBind, NewCommander);
					++SlotIndex;
				}
				else if (SlotIndex == 2 && SkillWidget_3)
				{
					bBindResult &= SkillWidget_3->BindToItem(SkillTagToBind, NewCommander);
					++SlotIndex;
				}
				else
				{
					break;
				}
			}
		}
		return bBindResult;
	}
	return false;

}


