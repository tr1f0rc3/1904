// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVInGameMainUserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "UserWidget.h"
#include "TextBlock.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "Pawns/KRVHero.h"
#include "Controllers/KRVInGamePlayerController.h"
#include "KRVGameState.h"

#include "KRVIconUserWidget.h"
#include "KRVInGameMainHeroUserWidget.h"
#include "KRVInGameCommanderUserWidget.h"
#include "KRVInGameHUDUserWidget.h"
#include "KRVInGameMenuUserWidget.h"
#include "KRVLogBoxUserWidget.h"
#include "KRVUnitInfoUserWidget.h"

void UKRVInGameMainUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	HeroWidgets = Cast<UKRVInGameMainHeroUserWidget>(GetWidgetFromName(TEXT("UI_InGame_MainHero_1")));
	CommanderWidgets = Cast<UKRVInGameCommanderUserWidget>(GetWidgetFromName(TEXT("UI_InGame_Commander_1")));
	ItemWidgets = Cast<UKRVInGameCommanderUserWidget>(GetWidgetFromName(TEXT("UI_InGame_Item_1")));
	HUDWidgets = Cast<UKRVInGameHUDUserWidget>(GetWidgetFromName(TEXT("UI_InGame_HUD_1")));
	LogBoxWidget = Cast<UKRVLogBoxUserWidget>(GetWidgetFromName(TEXT("UI_InGame_LogBox_1")));

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	MenuIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_MenuIcon_1")));
	KRVCHECK(MenuIconWidget);
	if (MenuIconWidget)
	{
		MenuIconWidget->SetCustomMenuIcon(EIconTextureVariation::Setting, FText::FromName(MenuIconTitle), FText::FromName(MenuIconText));
		MenuIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVInGameMainUserWidget::OnGameMenuIconClicked);

		Streamable.RequestAsyncLoad(GameMenuWidgetClassPath, FStreamableDelegate::CreateUObject(this,
			&UKRVInGameMainUserWidget::OnGameMenuWidgetClassAsyncLoaded));
	}

	PCTextBlock = Cast<UTextBlock>(GetWidgetFromName(TEXT("TB_PC_1")));
	if (PCTextBlock && GetWorld())
	{
		PCTextBlock->SetVisibility(ESlateVisibility::Hidden);
		AKRVInGamePlayerController* KRVInGamePlayerController =
			UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		BindPCTextBlockToPC(KRVInGamePlayerController);
	}

	{
		Streamable.RequestAsyncLoad(UnitInfoWidgetClassPath, FStreamableDelegate::CreateUObject(this,
			&UKRVInGameMainUserWidget::OnUnitInfoWidgetClassAsyncLoaded));
	}

}

bool UKRVInGameMainUserWidget::ShowThisWidget_Implementation(bool bIsTrue)
{
	ESlateVisibility CurrentVisbility = GetVisibility();
	if (bIsTrue && CurrentVisbility == ESlateVisibility::Hidden)
	{
		SetVisibility(ESlateVisibility::SelfHitTestInvisible);
		return true;
	}
	else if (!bIsTrue && CurrentVisbility == ESlateVisibility::SelfHitTestInvisible)
	{
		return true;
	}
	return false;

}

bool UKRVInGameMainUserWidget::BindToHero(AKRVHero* NewHero)
{
	if (!NewHero || !HeroWidgets || !LogBoxWidget)
	{
		KRVCHECK(NewHero);
		KRVCHECK(LogBoxWidget);
		return false;
	}
	else
	{
		LogBoxWidget->BindToHero(NewHero);
		return HeroWidgets->BindToHero(NewHero);
	}

}

bool UKRVInGameMainUserWidget::BindToCommander(AKRVCombatUnit* NewCommander)
{
	if (!NewCommander)
	{
		KRVCHECK(NewCommander);
	}
	else
	{
		bool bBindResult = true;
		if (!CommanderWidgets)
		{
			KRVCHECK(CommanderWidgets);
		}
		else
		{
			bBindResult = CommanderWidgets->BindToGlobalSkills(NewCommander);
		}
		if (!ItemWidgets)
		{
			KRVCHECK(ItemWidgets);
		}
		else
		{
			bBindResult = ItemWidgets->BindToItems(NewCommander);
		}
		return bBindResult;
	}
	return false;


}

bool UKRVInGameMainUserWidget::BindPCTextBlockToPC(class AKRVInGamePlayerController* NewController)
{
	if (!NewController)
	{
		KRVCHECK(NewController);
		return false;
	}
	else
	{
		NewController->OnTargetingBeginDelegate.AddUObject(this, &UKRVInGameMainUserWidget::OnTargetingBegin);
		NewController->OnTargetingEndMultiDelegate.AddUObject(this, &UKRVInGameMainUserWidget::OnTargetingEnd);
		return true;
	}

}

void UKRVInGameMainUserWidget::OnTargetingBegin(ETargetingType NewTargetingType)
{
	if (PCTextBlock)
	{
		switch (NewTargetingType)
		{
		case ETargetingType::Target:
		{
			PCTextBlock->SetText(FText::FromString(OnUnitTargetingBeginString));
			break;
		}
		case ETargetingType::Location:
		{
			PCTextBlock->SetText(FText::FromString(OnLocationTargetingBeginString));
			break;
		}
		default:
			break;
		}
		PCTextBlock->SetVisibility(ESlateVisibility::HitTestInvisible);
	}

}

void UKRVInGameMainUserWidget::OnTargetingEnd()
{
	if (PCTextBlock)
	{
		PCTextBlock->SetVisibility(ESlateVisibility::Hidden);
	}

}

void UKRVInGameMainUserWidget::OnGameMenuWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVInGameMenuUserWidget> AssetPathPtr(GameMenuWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		GameMenuWidgetClass = AssetPathPtr.Get();
	}

}

void UKRVInGameMainUserWidget::OnUnitInfoWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVUnitInfoUserWidget> AssetPathPtr(UnitInfoWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		UnitInfoWidgetClass = AssetPathPtr.Get();
		if (!UnitInfoWidget && UnitInfoWidgetClass)
		{
			UnitInfoWidget = CreateWidget<UKRVUnitInfoUserWidget>(this, UnitInfoWidgetClass);
			UnitInfoWidget->AddToViewport(2);

			if (GetWorld())
			{
				AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
				if (KRVInGamePlayerController)
				{
					BindUnitInfoWidgetToPC(KRVInGamePlayerController);
				}
			}
		}
	}

}

void UKRVInGameMainUserWidget::SetThisWidgetHiddenOnAnimFinished()
{
	SetVisibility(ESlateVisibility::Hidden);

}

void UKRVInGameMainUserWidget::OnGameMenuIconClicked()
{
	if (!GameMenuWidget && GameMenuWidgetClass)
	{
		GameMenuWidget = CreateWidget<UKRVInGameMenuUserWidget>(this, GameMenuWidgetClass);
		GameMenuWidget->AddToViewport(2);
	}

	if (!GameMenuWidget)
	{
		KRVCHECK(GameMenuWidget);
	}
	else if (GetWorld())
	{
		AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		if (KRVInGamePlayerController)
		{
			KRVInGamePlayerController->SetGameInputMode(false);
		}

		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
		GameMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}

}

bool UKRVInGameMainUserWidget::BindUnitInfoWidgetToPC(class AKRVInGamePlayerController* NewController)
{
	if (!UnitInfoWidget || !NewController)
	{
		KRVCHECK(UnitInfoWidget);
		KRVCHECK(NewController);
	}
	else
	{
		return UnitInfoWidget->BindToPC(NewController);
	}
	return false;

}
