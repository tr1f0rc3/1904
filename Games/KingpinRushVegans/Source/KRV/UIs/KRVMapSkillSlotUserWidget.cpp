// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVMapSkillSlotUserWidget.h"
#include "Border.h"
#include "TextBlock.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVDataAsset.h"
#include "Actors/KRVSkill.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "KRVIconUserWidget.h"

void UKRVMapSkillSlotUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UKRVMapSkillSlotUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	SkillIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_Skill_1")));
	LevelBorder = Cast<UBorder>(GetWidgetFromName(TEXT("BD_Level_1")));
	LevelTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_Level_1")));
	LearningIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_Learning_1")));

}

class UKRVIconUserWidget* UKRVMapSkillSlotUserWidget::GetSkillIconWidget() const
{
	return SkillIconWidget;

}

bool UKRVMapSkillSlotUserWidget::BindToSkillInMap(TSubclassOf<class AKRVSkill> SkillClassToBind, UKRVDataAsset* ItemDAToBind, const FName& NewHero)
{
	bIsBound = false;

	if (!GetWorld() || !SkillClassToBind || !ItemDAToBind || !SkillIconWidget || !LearningIconWidget || !LevelTextBlock)
	{
		KRVCHECK(ItemDAToBind);
		KRVCHECK(SkillIconWidget);
		KRVCHECK(LearningIconWidget);
		KRVCHECK(LevelTextBlock);
		SetIconLocked();
	}
	else
	{
		bIsBound = true;
		SkillDA = ItemDAToBind;
		HeroName = NewHero;

		bIsBound &= SkillIconWidget->BindToSkillInMap(SkillClassToBind, NewHero);
		bIsBound &= LearningIconWidget->BindToSkillLearning(SkillDA, NewHero);

		AKRVMapPlayerState* KRVMapPlayerState = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerState(GetWorld());
		if (!KRVMapPlayerState)
		{
			KRVCHECK(KRVMapPlayerState);
			bIsBound = false;
		}
		else if (LevelBorder && LevelTextBlock)
		{
			LevelBorder->SetVisibility(ESlateVisibility::Visible);
			OnPointGivenToHeroSkill(KRVMapPlayerState->GetPointOfHeroSkill(SkillDA, HeroName), SkillDA, HeroName);
			KRVMapPlayerState->OnPointGivenToHeroSkillDelegate.AddUObject(this, &UKRVMapSkillSlotUserWidget::OnPointGivenToHeroSkill);
		}
	}
	return false;

}

void UKRVMapSkillSlotUserWidget::SetIconLocked()
{
	bIsBound = false;
	if (SkillIconWidget)
	{
		SkillIconWidget->SetCustomMenuIcon(EIconTextureVariation::Locked);
		SkillIconWidget->SetVisibility(ESlateVisibility::Visible);
	}
	if (LevelBorder)
	{
		LevelBorder->SetVisibility(ESlateVisibility::Hidden);
	}
	if (LearningIconWidget)
	{
		LearningIconWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UKRVMapSkillSlotUserWidget::OnPointGivenToHeroSkill(int32 NewSkillPoint, class UKRVDataAsset* NewDA, const FName& NewHeroName)
{
	if (HeroName == NewHeroName)
	{
		if (SkillDA == NewDA)
		{
			if (!LevelTextBlock)
			{
				KRVCHECK(LevelTextBlock);
			}
			else
			{
				KRVCHECK(NewSkillPoint <= MaxSkillMasteryLevel);
				FString LevelString = FString::FromInt(NewSkillPoint) + FString(TEXT("/")) + FString::FromInt(MaxSkillMasteryLevel);
				LevelTextBlock->SetText(FText::FromString(LevelString));

				if (!SkillIconWidget)
				{
					KRVCHECK(SkillIconWidget);
				}
				else
				{
					KRVCHECK(SkillIconWidget->OnPointGivenToHeroSkill(NewSkillPoint, NewDA, NewHeroName));
				}
			}
		}
	}

}
