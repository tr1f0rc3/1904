// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVBattlefield.h"
#include "Engine/StreamableManager.h"
#include "Engine/AssetManager.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVLevelDataAsset.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "Controllers/KRVMapPlayerController.h"
#include "SaveGames/KRVSaveGame.h"
#include "DataAssets/KRVDataAsset.h"
#include "Components/BoxComponent.h"
#include "WidgetComponent.h"
#include "UIs/KRVIconUserWidget.h"

AKRVBattlefield::AKRVBattlefield()
{
	PrimaryActorTick.bCanEverTick = false;

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BOX_COMPONENT"));
	SetRootComponent(BoxComponent);
	BoxComponent->SetBoxExtent(100.f * FVector::OneVector, false);

	BattlefieldWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("INCOMING_WIDGET"));
	BattlefieldWidgetComponent->SetupAttachment(GetRootComponent());
	static ConstructorHelpers::FClassFinder<UKRVIconUserWidget> BATTLEFIELD_WIDGET_C(TEXT("/Game/Resources/UI/UI_IncomingButton.UI_IncomingButton_C"));
	if (BATTLEFIELD_WIDGET_C.Succeeded())
	{
		BattlefieldWidgetComponent->AddRelativeLocation(FVector(0.f, 0.f, 0.f));
		BattlefieldWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
		BattlefieldWidgetComponent->SetWidgetClass(BATTLEFIELD_WIDGET_C.Class);
		BattlefieldWidgetComponent->SetDrawSize(FVector2D(12.f, 12.f));
		BattlefieldWidgetComponent->SetHiddenInGame(true);
	}

}

void AKRVBattlefield::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(this);
	if (!KRVMapPlayerController)
	{
		KRVCHECK(KRVMapPlayerController);
	}
	else
	{
		OnBattlefieldIconClickedDelegate.BindUObject(KRVMapPlayerController, &AKRVMapPlayerController::OnBattlefieldIconClicked);
	}

}

void AKRVBattlefield::BeginPlay()
{
	Super::BeginPlay();

}

UKRVDataAsset* AKRVBattlefield::GetDataAsset_Implementation() const
{
	return Cast<UKRVDataAsset>(LevelDataAsset);

}

bool AKRVBattlefield::LoadFromDataAsset_Implementation()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	LevelDataAsset = Cast<UKRVLevelDataAsset>(Streamable.LoadSynchronous(SoftPtr_LevelDataAsset.ToSoftObjectPath(), true));
	if (LevelDataAsset)
	{
		return true;
	}
	return false;

}

UKRVLevelDataAsset* AKRVBattlefield::GetLevelDataAsset() const
{
	return LevelDataAsset;

}

bool AKRVBattlefield::ShowIconWidget(bool bIsShown)
{
	if (BattlefieldWidgetComponent)
	{
		BattlefieldWidgetComponent->SetHiddenInGame(!bIsShown);
		return true;
	}
	return false;

}

void AKRVBattlefield::OnIconClicked()
{
	if (!LevelDataAsset)
	{
		KRVCHECK(LevelDataAsset);
	}
	else
	{
		FKRVLevelSaveData KRVLevelSaveData;
		if (GetKRVLevelSaveData(KRVLevelSaveData, LevelDataAsset))
		{
			OnBattlefieldIconClickedDelegate.ExecuteIfBound(LevelDataAsset, KRVLevelSaveData);
		}
	}
}

bool AKRVBattlefield::BindToWidget()
{
	if (!LevelDataAsset || !BattlefieldWidgetComponent)
	{
		KRVCHECK(LevelDataAsset);
		KRVCHECK(BattlefieldWidgetComponent);
	}
	else
	{
		FKRVLevelSaveData KRVLevelSaveData;
		if (GetKRVLevelSaveData(KRVLevelSaveData, LevelDataAsset))
		{
			UKRVIconUserWidget* CastedBattlefieldWidget = Cast<UKRVIconUserWidget>(BattlefieldWidgetComponent->GetUserWidgetObject());
			if (!CastedBattlefieldWidget)
			{
				KRVCHECK(CastedBattlefieldWidget);
			}
			else
			{
				CastedBattlefieldWidget->BindToBattlefield(LevelDataAsset, KRVLevelSaveData);
				CastedBattlefieldWidget->OnIconClickedDelegate.AddUObject(this, &AKRVBattlefield::OnIconClicked);
			}
		}
	}
	return false;

}

bool AKRVBattlefield::GetKRVLevelSaveData(FKRVLevelSaveData& OutSaveData, UKRVLevelDataAsset* NewLevelDA)
{
	if (!NewLevelDA)
	{
		KRVCHECK(NewLevelDA);
	}
	else
	{
		AKRVMapPlayerState* KRVMapPlayerState = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerState(this);
		if (!KRVMapPlayerState)
		{
			KRVCHECK(KRVMapPlayerState);
		}
		else
		{
			UKRVSaveGame* KRVSaveGame = KRVMapPlayerState->GetSaveGame();
			if (!KRVSaveGame)
			{
				KRVCHECK(KRVSaveGame);
			}
			else
			{
				return KRVSaveGame->GetLevelSaveData(OutSaveData, LevelDataAsset->ClassFName.ToString());
			}
		}
	}
	return false;
}
