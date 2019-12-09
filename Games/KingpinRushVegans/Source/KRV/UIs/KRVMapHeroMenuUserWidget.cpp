// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVMapHeroMenuUserWidget.h"
#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVHeroDataAsset.h"
#include "SaveGames/KRVSaveGame.h"
#include "Actors/KRVSkill.h"
#include "KRVGameInstance.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "Controllers/KRVMapPlayerController.h"

#include "KRVIconUserWidget.h"
#include "KRVMapSkillSlotUserWidget.h"

void UKRVMapHeroMenuUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_1"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_2"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_3"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_4"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_5"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_6"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_7"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_8"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_9"))));
	HeroIcons.Emplace(Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_10"))));

	PickedHeroIcon = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_PickedHeroIcon_1")));

	ConfirmIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ConfirmIcon_1")));
	if (ConfirmIconWidget)
	{
		ConfirmIconWidget->SetCustomMenuIcon(EIconTextureVariation::Confirm);
		ConfirmIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapHeroMenuUserWidget::OnConfirmIconClicked);
	}

	CancleIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_CancleIcon_1")));
	if (CancleIconWidget)
	{
		CancleIconWidget->SetCustomMenuIcon(EIconTextureVariation::Cancle);
		CancleIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapHeroMenuUserWidget::OnCancleIconClicked);
	}

	SkillSlots.Emplace(Cast<UKRVMapSkillSlotUserWidget>(GetWidgetFromName(TEXT("UI_SkillSlot_1"))));
	SkillSlots.Emplace(Cast<UKRVMapSkillSlotUserWidget>(GetWidgetFromName(TEXT("UI_SkillSlot_2"))));
	SkillSlots.Emplace(Cast<UKRVMapSkillSlotUserWidget>(GetWidgetFromName(TEXT("UI_SkillSlot_3"))));
	SkillSlots.Emplace(Cast<UKRVMapSkillSlotUserWidget>(GetWidgetFromName(TEXT("UI_SkillSlot_4"))));

	ResetSkillPointsIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ResetSkillIcon_1")));
	if (ResetSkillPointsIconWidget)
	{
		ResetSkillPointsIconWidget->SetCustomMenuIcon(EIconTextureVariation::Default, FText::FromString(SkillResetTitle), FText::FromString(SkillResetText));
		ResetSkillPointsIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapHeroMenuUserWidget::OnResetSkillButtonClicked);
	}

}

void UKRVMapHeroMenuUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Visible);

}

void UKRVMapHeroMenuUserWidget::HideThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVMapHeroMenuUserWidget::OnConfirmIconClicked()
{
	if (GetWorld())
	{
		AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
		if (KRVMapPlayerController)
		{
			KRVMapPlayerController->SaveSaveGame();
		}
	}
	HideThisWidget();

}

void UKRVMapHeroMenuUserWidget::OnCancleIconClicked()
{
	HideThisWidget();

}


void UKRVMapHeroMenuUserWidget::OnHeroPicked(const FName& EquippedHero)
{
	int32* EquippedHeroIndexPtr = HeroIndexMap.Find(EquippedHero);
	if (!EquippedHeroIndexPtr || !PickedHeroIcon)
	{
		KRVCHECK(EquippedHeroIndexPtr);
		KRVCHECK(PickedHeroIcon);
	}
	else
	{
		FName PickedHeroName;
		PickedHeroIcon->GetItemName(PickedHeroName);

		if (PickedHeroName != EquippedHero)
		{
			int32 EquippedHeroIndex = *EquippedHeroIndexPtr;
			if (!HeroDAArray.IsValidIndex(EquippedHeroIndex) || !HeroSaveDataArray.IsValidIndex(EquippedHeroIndex))
			{
				KRVCHECK(HeroDAArray.IsValidIndex(EquippedHeroIndex));
				KRVCHECK(HeroSaveDataArray.IsValidIndex(EquippedHeroIndex));
			}
			else
			{
				PickedHeroIcon->BindToHeroInMap
				(HeroDAArray[EquippedHeroIndex], HeroSaveDataArray[EquippedHeroIndex], true);
			}
		}
	}

}

void UKRVMapHeroMenuUserWidget::OnHeroClicked(const FName& NewClickedHero)
{
	if (ClickedHeroName != NewClickedHero)
	{
		int32* EquippedHeroIndexPtr = HeroIndexMap.Find(NewClickedHero);
		UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(GetWorld());

		if (!EquippedHeroIndexPtr || KRVGameInstance)
		{
			KRVCHECK(EquippedHeroIndexPtr);
			KRVCHECK(KRVGameInstance);
		}
		else
		{
			int32 EquippedHeroIndex = *EquippedHeroIndexPtr;
			if (!HeroDAArray.IsValidIndex(EquippedHeroIndex) || !HeroSaveDataArray.IsValidIndex(EquippedHeroIndex))
			{
				KRVCHECK(HeroDAArray.IsValidIndex(EquippedHeroIndex));
				KRVCHECK(HeroSaveDataArray.IsValidIndex(EquippedHeroIndex));
			}
			else
			{
				UKRVHeroDataAsset* ClickedHeroDA = HeroDAArray[EquippedHeroIndex];
				FKRVHeroSaveData& ClickedHeroSaveData = HeroSaveDataArray[EquippedHeroIndex];

				if (!ClickedHeroDA || !KRVGameInstance)
				{
					KRVCHECK(ClickedHeroDA);
					KRVCHECK(KRVGameInstance);
				}
				else
				{
					ClickedHeroName = NewClickedHero;
					for (int32 Index = 0; Index < MaxSkillsToLearn && SkillSlots.IsValidIndex(Index); ++Index)
					{
						bool bIsSkillToLearnBound = false;
						UKRVMapSkillSlotUserWidget* SkillSlot = SkillSlots[Index];
						if (ClickedHeroDA->SkillClass.IsValidIndex(Index))
						{
							TSubclassOf<AKRVSkill> KRVSkillClass = ClickedHeroDA->SkillClass[Index];
							AKRVSkill* KRVSkill = KRVSkillClass.GetDefaultObject();
							if (!KRVSkill)
							{
								KRVCHECK(KRVSkill);
							}
							else
							{
								UKRVDataAsset* SkillDataAsset = KRVSkill->GetDataAsset_Implementation();
								if (!SkillDataAsset)
								{
									KRVCHECK(SkillDataAsset);
								}
								else
								{
									SkillSlot->BindToSkillInMap(KRVSkillClass, SkillDataAsset, ClickedHeroName);
									bIsSkillToLearnBound = true;
								}
							}
						}

						if (!bIsSkillToLearnBound)
						{
							SkillSlot->SetIconLocked();
						}
					}
				}
			}
		}
	}

}

void UKRVMapHeroMenuUserWidget::OnResetSkillButtonClicked()
{
	if (GetWorld())
	{
		AKRVMapPlayerController* KRVMapPC = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(GetWorld());
		if (!KRVMapPC || !PickedHeroIcon)
		{
			KRVCHECK(KRVMapPC);
			KRVCHECK(PickedHeroIcon);
		}
		else if (PickedHeroIcon->IsBound())
		{
			FName PickedHeroName;
			PickedHeroIcon->GetItemName(PickedHeroName);
			KRVMapPC->ResetHeroSkillPoints(PickedHeroName);
		}
	}

}

void UKRVMapHeroMenuUserWidget::BindToPlayerState(class AKRVMapPlayerState* PlayerStateToBind)
{
	if (!PlayerStateToBind)
	{
		KRVCHECK(PlayerStateToBind);
	}
	else if (GetWorld())
	{
		UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(GetWorld());
		UKRVSaveGame* KRVSaveGame = PlayerStateToBind->GetSaveGame();
		if (!KRVSaveGame || !KRVGameInstance)
		{
			KRVCHECK(KRVSaveGame);
			KRVCHECK(KRVGameInstance);
		}
		else
		{
			// Bind delegate
			PlayerStateToBind->OnHeroPickedDelegate.AddUObject(this, &UKRVMapHeroMenuUserWidget::OnHeroPicked);

			// Bind Hero data
			for (int32 Index = 0; HeroIcons.IsValidIndex(Index) &&
				KRVSaveGame->HeroSaveData.IsValidIndex(Index); ++Index)
			{
				const FKRVHeroSaveData& HeroSaveDataToBind = KRVSaveGame->HeroSaveData[Index];
				const FName& HeroNameToBind = HeroSaveDataToBind.HeroName;
				UKRVIconUserWidget* HeroIconToBind = HeroIcons[Index];
				UKRVHeroDataAsset* HeroDAToBind = KRVGameInstance->GetHeroDA(HeroSaveDataToBind.HeroName);
				if (!HeroDAToBind)
				{
					KRVCHECK(HeroDAToBind);
				}
				else
				{
					// save data to array
					int32 HeroDAEmplaceIndex = HeroDAArray.Emplace(HeroDAToBind);
					int32 HeroSaveDataEmplaceIndex = HeroSaveDataArray.Emplace(HeroSaveDataToBind);
					KRVCHECK(HeroDAEmplaceIndex == HeroSaveDataEmplaceIndex);
					HeroIndexMap.Emplace(HeroNameToBind, HeroDAEmplaceIndex);

					// binding
					HeroIconToBind->BindToHeroInMap(HeroDAToBind, HeroSaveDataToBind);
					HeroIconToBind->OnHeroInMapClickedDelegate.AddUObject(this, &UKRVMapHeroMenuUserWidget::OnHeroClicked);
					if (PlayerStateToBind->IsPickedHero(HeroNameToBind))
					{
						HeroIconToBind->OnHeroPicked(HeroNameToBind);
						OnHeroPicked(HeroNameToBind);
					}
				}
			}
		}
	}

}
