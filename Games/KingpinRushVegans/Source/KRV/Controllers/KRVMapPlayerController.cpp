// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVMapPlayerController.h"
#include "EngineUtils.h"

#include "KRV.h"
#include "Structs/KRVHeroSaveData.h"
#include "DataAssets/KRVLevelDataAsset.h"
#include "DataAssets/KRVHeroDataAsset.h"
#include "SaveGames/KRVSaveGame.h"
#include "SaveGames/KRVInGameSave.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "Actors/KRVSkill.h"
#include "Actors/KRVBattleField.h"

#include "UIs/KRVLevelInfoUserWidget.h"
#include "UIs/KRVMapMainUserWidget.h"

AKRVMapPlayerController::AKRVMapPlayerController()
{
	static ConstructorHelpers::FClassFinder<UKRVMapMainUserWidget> MAP_MAIN_UI_C
	(TEXT("/Game/Resources/UI/UI_MapMain.UI_MapMain_C"));
	if (MAP_MAIN_UI_C.Succeeded())
	{
		MapMainWidgetClass = MAP_MAIN_UI_C.Class;
	}

	static ConstructorHelpers::FClassFinder<UKRVLevelInfoUserWidget> LEVEL_INFO_UI_C
	(TEXT("/Game/Resources/UI/UI_LevelInfo.UI_LevelInfo_C"));
	if (LEVEL_INFO_UI_C.Succeeded())
	{
		LevelInfoWidgetClass = LEVEL_INFO_UI_C.Class;
	}


}

void AKRVMapPlayerController::BeginPlay()
{
	Super::BeginPlay();

	GetKRVMapPlayerState()->ApplyInGameSaveToSaveGame();


}

AKRVMapPlayerState* AKRVMapPlayerController::GetKRVMapPlayerState()
{
	if (!KRVMapPlayerState)
	{
		KRVMapPlayerState = Cast<AKRVMapPlayerState>(PlayerState);
	}
	return KRVMapPlayerState;

}

class UKRVSaveGame* AKRVMapPlayerController::GetSaveGame()
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->GetSaveGame();
	}
	return false;

}

bool AKRVMapPlayerController::SaveSaveGame()
{
	return GetKRVMapPlayerState()->SaveSaveGame();

}

void AKRVMapPlayerController::OnBattlefieldIconClicked(UKRVLevelDataAsset* NewLevelDA, const FKRVLevelSaveData& NewLevelSaveDA)
{
	if (!LevelInfoWidget && LevelInfoWidgetClass)
	{
		LevelInfoWidget = CreateWidget<UKRVLevelInfoUserWidget>(this, LevelInfoWidgetClass);
		AddWidgetFromViewport(LevelInfoWidget, true, 2);
	}

	if (!LevelInfoWidget)
	{
		KRVCHECK(LevelInfoWidget);
	}
	else if (BindWidgetToLevel(NewLevelDA, NewLevelSaveDA))
	{
		LevelInfoWidget->ShowThisWidget();
	}

}

bool AKRVMapPlayerController::BindWidgetToLevel(UKRVLevelDataAsset* NewLevelDA, const FKRVLevelSaveData& NewLevelSaveDA)
{
	if (!LevelInfoWidget || !NewLevelDA)
	{
		KRVCHECK(LevelInfoWidget);
		KRVCHECK(NewLevelDA);
	}
	else
	{
		LevelInfoWidget->BindToLevel(NewLevelDA, NewLevelSaveDA);
		return true;
	}
	return false;

}

void AKRVMapPlayerController::ShowBattlefieldWidgets()
{
	if (GetWorld())
	{
		for (TActorIterator<AKRVBattlefield> ActorItr(GetWorld()); ActorItr; ++ActorItr)
		{
			AKRVBattlefield* KRVBattlefield = *ActorItr;
			if (KRVBattlefield)
			{
				KRVBattlefield->ShowIconWidget(true);
			}
		}
	}

}

bool AKRVMapPlayerController::OpenInGameLevelByDA(const class UKRVLevelDataAsset* LevelDAToOpen, const FKRVLevelSaveData& LevelSaveDataToOpen)
{
	if (!LevelDAToOpen)
	{
		KRVCHECK(LevelDAToOpen);
	}
	else
	{
		return OpenInGameLevelByString(LevelDAToOpen->ClassFName.ToString(), LevelSaveDataToOpen);
	}
	return false;

}

bool AKRVMapPlayerController::OpenInGameLevelByString(const FString& LevelStringToOpen, const FKRVLevelSaveData& LevelSaveDataToOpen)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else if (!LevelSaveDataToOpen.bIsUnlocked)
	{
		KRVLOG(Warning, TEXT("It is locked level. Opening terminated."));
	}
	else if (GetWorld())
	{
		ReservedLevelStringToOpen = LevelStringToOpen;
		GetWorldTimerManager().SetTimer(FadingTimer, this,
			&AKRVMapPlayerController::OpenInGameLevelAfterFadeOut, FadeDuration, false);
		return true;
	}
	return false;

}

void AKRVMapPlayerController::RequestToShowUI()
{
	bShowMouseCursor = false;
	FadeInTheScreen(true);
	if (GetWorld())
	{
		GetWorldTimerManager().SetTimer(FadingTimer, this,
			&AKRVMapPlayerController::ShowUIAfterFadeIn, FadeDuration, false);
	}

	if (!MapMainWidget && MapMainWidgetClass)
	{
		MapMainWidget = CreateWidget<UKRVMapMainUserWidget>(this, MapMainWidgetClass);
		AddWidgetFromViewport(MapMainWidget, true, 1);
		MapMainWidget->SetVisibility(ESlateVisibility::Hidden);
	}

}

void AKRVMapPlayerController::ShowUIAfterFadeIn()
{
	SetGameInputMode(false);

	if (MapMainWidget)
	{
		MapMainWidget->ShowThisWidget();
	}

	ShowBattlefieldWidgets();

}

void AKRVMapPlayerController::OpenInGameLevelAfterFadeOut()
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else if (GetKRVMapPlayerState()->UpdateInGameSaveFromSaveGame(ReservedLevelStringToOpen))
	{
		OpenLevelByName(FName(*ReservedLevelStringToOpen));
	}

}

bool AKRVMapPlayerController::PickHero(const FName& NewHeroName)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->PickHero(NewHeroName);
	}
	return false;

}

bool AKRVMapPlayerController::GivePointToHeroSkill(class UKRVDataAsset* NewDA, const FName& NewHeroName)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->GivePointToHeroSkill(NewDA, NewHeroName);
	}
	return false;
	
}

bool AKRVMapPlayerController::ResetHeroSkillPoints(const FName& NewHeroName)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->ResetHeroSkillPoints(NewHeroName);
	}
	return false;

}

bool AKRVMapPlayerController::EquipItem(const FName& ItemToEquip)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->EquipItem(ItemToEquip);
	}
	return false;

}

bool AKRVMapPlayerController::UnequipItem(const FName& ItemToUnequip)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->UnequipItem(ItemToUnequip);
	}
	return false;
}

bool AKRVMapPlayerController::BuyItem(const class UKRVItemDataAsset* ItemDAToBuy, int32 AmountToBuy /*= 1*/)
{
	if (!GetKRVMapPlayerState() || !ItemDAToBuy)
	{
		KRVCHECK(KRVMapPlayerState);
		KRVCHECK(ItemDAToBuy);
	}
	else
	{
		return GetKRVMapPlayerState()->BuyItem(ItemDAToBuy, AmountToBuy);
	}
	return false;
}

bool AKRVMapPlayerController::EquipTower(const FName& TowerToEquip)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->EquipTower(TowerToEquip);
	}
	return false;

}

bool AKRVMapPlayerController::UnequipTower(const FName& TowerToUnequip)
{
	if (!GetKRVMapPlayerState())
	{
		KRVCHECK(KRVMapPlayerState);
	}
	else
	{
		return GetKRVMapPlayerState()->EquipTower(TowerToUnequip);
	}
	return false;

}

