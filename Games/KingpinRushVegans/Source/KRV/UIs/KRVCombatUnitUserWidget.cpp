// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVCombatUnitUserWidget.h"
#include "ProgressBar.h"

#include "KRV.h"
#include "KRVPBUserWidget.h"
#include "Actors/KRVSkill.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"

void UKRVCombatUnitUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

}

void UKRVCombatUnitUserWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	HPBarWidget = Cast<UKRVPBUserWidget>(GetWidgetFromName(TEXT("UI_HPBar_1")));
	HPBarWidget->SetVisibility(ESlateVisibility::Collapsed);
	CastingBarWidget = Cast<UKRVPBUserWidget>(GetWidgetFromName(TEXT("UI_CastingBar_1")));
	CastingBarWidget->SetVisibility(ESlateVisibility::Collapsed);

}

bool UKRVCombatUnitUserWidget::BindToCharacter(AKRVCharacter* NewCharacter)
{
	if (!NewCharacter)
	{
		KRVCHECK(NewCharacter);
	}
	else
	{
		bool bBindResult = true;
		if (!HPBarWidget)
		{
			bBindResult = false;
		}

		bBindResult &= BindToCombatUnit(Cast<AKRVCombatUnit>(NewCharacter));
		if (HPBarWidget)
		{
			bBindResult &= HPBarWidget->BindToHealth(NewCharacter->GetModifiedCharacterStat());
			HPBarWidget->NeedToShowDelegate.BindUObject(this, &UKRVCombatUnitUserWidget::ShowHPBarWidget);
		}
		return bBindResult;
	}
	return false;

}

bool UKRVCombatUnitUserWidget::BindToCombatUnit(class AKRVCombatUnit* NewCombatUnit)
{
	if (!NewCombatUnit)
	{
		KRVCHECK(NewCombatUnit);
	}
	else
	{
		bool bBindResult = true;
		if (!CastingBarWidget)
		{
			bBindResult = false;
		}
		else
		{
			bBindResult &= CastingBarWidget->BindToCasting(NewCombatUnit);
			CastingBarWidget->NeedToShowDelegate.BindUObject(this, &UKRVCombatUnitUserWidget::ShowCastingBarWidget);
		}
		return bBindResult;
	}
	return false;

}

void UKRVCombatUnitUserWidget::ShowHPBarWidget(bool bIsTrue)
{
	if (!HPBarWidget)
	{
		KRVCHECK(HPBarWidget);
	}
	else if (bIsTrue)
	{
		HPBarWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		HPBarWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

}

void UKRVCombatUnitUserWidget::ShowCastingBarWidget(bool bIsTrue)
{
	if (!CastingBarWidget)
	{
		KRVCHECK(CastingBarWidget);
	}
	else if (bIsTrue)
	{
		CastingBarWidget->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		CastingBarWidget->SetVisibility(ESlateVisibility::Collapsed);
	}

}

