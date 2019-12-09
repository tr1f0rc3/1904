// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVInGameMainHeroUserWidget.h"
#include "TextBlock.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "ActorComponents\KRVCharacterStatComponent.h"
#include "Pawns/KRVHero.h"
#include "Actors/KRVSkill.h"
#include "PlayerStates/KRVInGamePlayerState.h"

#include "KRVIconUserWidget.h"
#include "KRVPBUserWidget.h"


void UKRVInGameMainHeroUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HeroIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_1")));
	SkillWidget_1 = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_SkillIcon_1")));
	SkillWidget_2 = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_SkillIcon_2")));
	HPBarWidget = Cast<UKRVPBUserWidget>(GetWidgetFromName(TEXT("UI_HPBar_1")));
	ExpBarWidget = Cast<UKRVPBUserWidget>(GetWidgetFromName(TEXT("UI_ExpBar_1")));

}

bool UKRVInGameMainHeroUserWidget::BindToHero(AKRVHero* NewHero)
{
	if (!NewHero)
	{
		KRVCHECK(NewHero);
	}
	else
	{
		BoundHero = NewHero;
		bool bBindResult = true;

		if (!HeroIconWidget || !SkillWidget_1 || !SkillWidget_2 || !HPBarWidget || !ExpBarWidget)
		{
			bBindResult = false;
		}

		if (HeroIconWidget)
		{
			bBindResult &= HeroIconWidget->BindToHero(NewHero);
		}

		// Bind skills
		for (int32 SkillIndex = 0, SlotIndex = 0; SlotIndex < MaxSkillSlotsToActive; ++SkillIndex)
		{
			FGameplayTag SkillTagToBind;
			if (!NewHero->GetSkillTagByIndex(SkillTagToBind, SkillIndex))
			{
				break;
			}
			else
			{
				AKRVSkill* SkillToCheck = NewHero->GetSkillByTag(SkillTagToBind);
				if (!SkillToCheck || SkillToCheck->IsAutoCastable() || SkillToCheck->GetTargetingType() == ETargetingType::Passive)
				{
					KRVCHECK(SkillToCheck);
					continue;
				}
				else if (SlotIndex == 0 && SkillWidget_1)
				{
					bBindResult &= SkillWidget_1->BindToSkill(SkillTagToBind, NewHero);
					++SlotIndex;
				}
				else if (SlotIndex == 1 && SkillWidget_2)
				{
					bBindResult &= SkillWidget_2->BindToSkill(SkillTagToBind, NewHero);
					++SlotIndex;
				}
				else
				{
					break;
				}
			}
		}

		// Bind bars
		if (HPBarWidget)
		{
			bBindResult &= HPBarWidget->BindToHealth(NewHero->GetModifiedCharacterStat());
		}
		if (ExpBarWidget)
		{
			bBindResult &= ExpBarWidget->BindToExp();
		}
		return bBindResult;
	}
	return false;

}
