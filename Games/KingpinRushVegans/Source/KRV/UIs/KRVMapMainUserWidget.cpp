// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVMapMainUserWidget.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"

#include "KRV.h"
#include "DataAssets/KRVLevelDataAsset.h"
#include "KRVIconUserWidget.h"
#include "KRVInGameMenuUserWidget.h"
#include "KRVMapItemMenuUserWidget.h"
#include "KRVMapTowerMenuUserWidget.h"
#include "KRVMapHeroMenuUserWidget.h"

void UKRVMapMainUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();

	MenuIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_MenuIcon_1")));
	if (!MenuIconWidget)
	{
		KRVCHECK(MenuIconWidget);
	}
	else
	{
		MenuIconWidget->SetCustomMenuIcon(EIconTextureVariation::Setting, FText::FromName(GameMenuTitle), FText::FromName(GameMenuText));
		MenuIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapMainUserWidget::OnGameMenuIconClicked);

		Streamable.RequestAsyncLoad(GameMenuWidgetClassPath, FStreamableDelegate::CreateUObject(this,
			&UKRVMapMainUserWidget::OnGameMenuWidgetClassAsyncLoaded));
	}

	HeroIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_HeroIcon_1")));
	if (!HeroIconWidget)
	{
		KRVCHECK(HeroIconWidget);
	}
	else
	{
		HeroIconWidget->SetCustomMenuIcon(EIconTextureVariation::Default, FText::FromName(HeroMenuTitle), FText::FromName(HeroMenuText));
		HeroIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapMainUserWidget::OnHeroMenuIconClicked);

		Streamable.RequestAsyncLoad(HeroMenuWidgetClassPath, FStreamableDelegate::CreateUObject(this,
			&UKRVMapMainUserWidget::OnHeroMenuWidgetClassAsyncLoaded));
	}

	ItemIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_ItemIcon_1")));
	if (!ItemIconWidget)
	{
		KRVCHECK(ItemIconWidget);
	}
	else
	{
		ItemIconWidget->SetCustomMenuIcon(EIconTextureVariation::Default, FText::FromName(ItemMenuTitle), FText::FromName(ItemMenuText));
		ItemIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapMainUserWidget::OnItemMenuIconClicked);

		Streamable.RequestAsyncLoad(ItemMenuWidgetClassPath, FStreamableDelegate::CreateUObject(this,
			&UKRVMapMainUserWidget::OnItemMenuWidgetClassAsyncLoaded));
	}

	TowerIconWidget = Cast<UKRVIconUserWidget>(GetWidgetFromName(TEXT("UI_TowerIcon_1")));
	if (!TowerIconWidget)
	{
		KRVCHECK(TowerIconWidget);
	}
	else
	{
		TowerIconWidget->SetCustomMenuIcon(EIconTextureVariation::Default, FText::FromName(TowerMenuTitle), FText::FromName(TowerMenuText));
		TowerIconWidget->OnIconClickedDelegate.AddUObject(this, &UKRVMapMainUserWidget::OnTowerMenuIconClicked);

		Streamable.RequestAsyncLoad(TowerMenuWidgetClassPath, FStreamableDelegate::CreateUObject(this,
			&UKRVMapMainUserWidget::OnTowerMenuWidgetClassAsyncLoaded));
	}

}

void UKRVMapMainUserWidget::ShowThisWidget_Implementation()
{
	SetVisibility(ESlateVisibility::HitTestInvisible);

}

void UKRVMapMainUserWidget::OnGameMenuWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVInGameMenuUserWidget> AssetPathPtr(GameMenuWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		GameMenuWidgetClass = AssetPathPtr.Get();
	}

}

void UKRVMapMainUserWidget::OnHeroMenuWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVMapHeroMenuUserWidget> AssetPathPtr(HeroMenuWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		HeroMenuWidgetClass = AssetPathPtr.Get();
	}

}

void UKRVMapMainUserWidget::OnItemMenuWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVMapItemMenuUserWidget> AssetPathPtr(ItemMenuWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		ItemMenuWidgetClass = AssetPathPtr.Get();
	}

}

void UKRVMapMainUserWidget::OnTowerMenuWidgetClassAsyncLoaded()
{
	TSoftClassPtr<UKRVMapTowerMenuUserWidget> AssetPathPtr(TowerMenuWidgetClassPath);
	KRVCHECK(AssetPathPtr.IsValid());
	if (AssetPathPtr.IsValid())
	{
		TowerMenuWidgetClass = AssetPathPtr.Get();
	}

}

void UKRVMapMainUserWidget::BindToPlayerState(class AKRVMapPlayerState* NewPlayerState)
{
	if (ItemMenuWidget)
	{
		ItemMenuWidget->BindToPlayerState(NewPlayerState);
	}
	if (HeroMenuWidget)
	{
		HeroMenuWidget->BindToPlayerState(NewPlayerState);
	}
	if (TowerMenuWidget)
	{
		TowerMenuWidget->BindToPlayerState(NewPlayerState);
	}

}

void UKRVMapMainUserWidget::OnGameMenuIconClicked()
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
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
		GameMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}

}

void UKRVMapMainUserWidget::OnHeroMenuIconClicked()
{
	if (!HeroMenuWidget && HeroMenuWidgetClass)
	{
		HeroMenuWidget = CreateWidget<UKRVMapHeroMenuUserWidget>(this, HeroMenuWidgetClass);
		HeroMenuWidget->AddToViewport(2);
	}

	if (!HeroMenuWidget)
	{
		KRVCHECK(HeroMenuWidget);
	}
	else if (GetWorld())
	{
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
		HeroMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}

}

void UKRVMapMainUserWidget::OnItemMenuIconClicked()
{
	if (!ItemMenuWidget && ItemMenuWidgetClass)
	{
		ItemMenuWidget = CreateWidget<UKRVMapItemMenuUserWidget>(this, ItemMenuWidgetClass);
		ItemMenuWidget->AddToViewport(2);
	}

	if (!ItemMenuWidget)
	{
		KRVCHECK(ItemMenuWidget);
	}
	else if (GetWorld())
	{
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
		ItemMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}


}

void UKRVMapMainUserWidget::OnTowerMenuIconClicked()
{
	if (!TowerMenuWidget && TowerMenuWidgetClass)
	{
		TowerMenuWidget = CreateWidget<UKRVMapTowerMenuUserWidget>(this, TowerMenuWidgetClass);
		TowerMenuWidget->AddToViewport(2);
	}

	if (!TowerMenuWidget)
	{
		KRVCHECK(TowerMenuWidget);
	}
	else if (GetWorld())
	{
		if (!UGameplayStatics::IsGamePaused(GetWorld()))
		{
			UGameplayStatics::SetGamePaused(GetWorld(), true);
		}
		TowerMenuWidget->SetVisibility(ESlateVisibility::Visible);
	}

}

