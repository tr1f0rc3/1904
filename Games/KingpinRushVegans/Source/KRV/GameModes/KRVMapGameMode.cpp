// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVMapGameMode.h"
#include "KRV.h"
#include "Pawns/KRVPlayerCameraPawn.h"
#include "Controllers/KRVMapPlayerController.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "PlayerStates/KRVInGamePlayerState.h"

AKRVMapGameMode::AKRVMapGameMode()
{
	DefaultPawnClass = AKRVPlayerCameraPawn::StaticClass();
	PlayerControllerClass = AKRVMapPlayerController::StaticClass();
	PlayerStateClass = AKRVMapPlayerState::StaticClass();

}

void AKRVMapGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPlayer->PlayerState);
	KRVCHECK(KRVInGamePlayerState);
	if (KRVInGamePlayerState)
	{
		KRVInGamePlayerState->LoadInGameSave();
	}

}

int32 AKRVMapGameMode::GetAvaialbleSkillPointsFromHeroLevel(int32 NewHeroLevel) const
{
	return 2 + NewHeroLevel;

}
