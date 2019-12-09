// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVGameState.h"
#include "Kismet/KismetSystemLibrary.h"

#include "KRV.h"
#include "KRVEnums.h"
#include "PlayerStates/KRVInGamePlayerState.h"
#include "Controllers/KRVInGamePlayerController.h"
#include "Controllers/KRVPlayerAIController.h"
#include "Pawns/KRVCharacter.h"

AKRVGameState::AKRVGameState()
{
}

int32 AKRVGameState::GetAvailableGold() const
{
	return AvailableGold;

}

void AKRVGameState::AddGold(int32 GoldToAdd)
{
	AvailableGold += GoldToAdd;
	OnGoldChangedDelegate.Broadcast(AvailableGold);

}

bool AKRVGameState::SpendGold(AKRVInGamePlayerController* NewPC, int32 NewSpentGold)
{
	if (!NewPC || NewSpentGold <= 0)
	{
		KRVCHECK(NewPC);
		KRVCHECK(NewSpentGold > 0);
		return false;
	}
	if (AvailableGold >= NewSpentGold)
	{
		AddGold(-NewSpentGold);
		AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPC->PlayerState);
		KRVCHECK(KRVInGamePlayerState);
		if (KRVInGamePlayerState)
		{
			KRVInGamePlayerState->AddSpentGold(NewSpentGold);
		}
		return true;
	}
	return false;

}

bool AKRVGameState::SetGameResultOfPlayerState(AKRVInGamePlayerController* NewPC, EKRVTeam NewWinner, int32 NewEarnedStars /*= 0*/)
{
	if (!NewPC)
	{
		KRVCHECK(NewPC);
	}
	else
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPC->PlayerState);
		KRVCHECK(KRVInGamePlayerState);
		if (KRVInGamePlayerState)
		{
			SecondsAtWaveCalled.Emplace(UKismetSystemLibrary::GetGameTimeInSeconds(this));

			switch (NewWinner)
			{
			case EKRVTeam::Player:
			{
				EarnedStars = NewEarnedStars;
				KRVInGamePlayerState->SetClearData(NewEarnedStars, GetSecondsAfterFirstWaveCalling());
				break;
			}
			case EKRVTeam::Enemy:
			{
				// not implemented
				break;
			}
			default:
				break;
			}
			return true;
		}
	}
	return false;


}

bool AKRVGameState::SavePlayerState(AKRVInGamePlayerController* NewPC)
{
	if (!NewPC)
	{
		KRVCHECK(NewPC);
	}
	else
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPC->PlayerState);
		if (!KRVInGamePlayerState)
		{
			KRVCHECK(KRVInGamePlayerState);
		}
		else
		{
			KRVInGamePlayerState->SaveInGameSave();
			return true;
		}
	}
	return false;

}

void AKRVGameState::AddRecruits(AKRVInGamePlayerController* NewPC, int32 NewRecruits)
{
	if (!NewPC)
	{
		KRVCHECK(NewPC);
	}
	else
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPC->PlayerState);
		if (!KRVInGamePlayerState)
		{
			KRVCHECK(KRVInGamePlayerState);
		}
		else
		{
			KRVInGamePlayerState->AddRecruits(NewRecruits);
		}
	}

}

void AKRVGameState::GainKillRewards(AKRVInGamePlayerController* NewPC, int32 GoldToEarn, int32 ExpToGet, int32 RubyToEarn /*= 0*/, int32 KillCountsToAdd /*= 1*/)
{
	if (!NewPC)
	{
		KRVCHECK(NewPC);
	}
	else
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPC->PlayerState);
		KRVCHECK(KRVInGamePlayerState);
		if (KRVInGamePlayerState)
		{
			KRVInGamePlayerState->GainKillRewards(GoldToEarn, ExpToGet, KillCountsToAdd, RubyToEarn);

		}
		TotalKills += KillCountsToAdd;
	}

}

void AKRVGameState::EarnRuby(AKRVInGamePlayerController* NewPC, int32 RubyToAdd)
{
	if (!NewPC)
	{
		KRVCHECK(NewPC);
		return;
	}
	AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPC->PlayerState);
	KRVCHECK(KRVInGamePlayerState);
	if (KRVInGamePlayerState)
	{
		EarnedRuby += RubyToAdd;
		KRVInGamePlayerState->EarnRuby(RubyToAdd);
	}

}

int32 AKRVGameState::GetEarnedRuby() const
{
	return EarnedRuby;

}

int32 AKRVGameState::GetEarnedStars() const
{
	return EarnedStars;

}

int32 AKRVGameState::GetHeartsRemaining() const
{
	return HeartsRemaining;

}

int32 AKRVGameState::AddEscapedEnemies(int32 Enemies)
{
	EscapedEnemies += Enemies;
	return EscapedEnemies;

}

int32 AKRVGameState::AddHearts(int32 HeartsToAdd)
{
	HeartsRemaining += HeartsToAdd;
	HeartsRemaining = FMath::Clamp(HeartsRemaining, 0, HeartsRemaining);
	OnHeartChangedDelegate.Broadcast(HeartsRemaining);
	return HeartsRemaining;

}

void AKRVGameState::SetHearts(int32 HeartsToSet)
{
	HeartsRemaining = HeartsToSet;

}

int32 AKRVGameState::GetCurrentWave() const
{
	return CurrentWave;

}


int32 AKRVGameState::GetTotalWaves() const
{
	return TotalWaves;

}

int32 AKRVGameState::JumpToNextWave(int32 WavesToJump /*= 1*/)
{
	CurrentWave += WavesToJump;
	OnWaveChangedDelegate.Broadcast(CurrentWave, TotalWaves);
	return CurrentWave;

}

EGameState AKRVGameState::GetGameStateEnum() const
{
	return InGameState;

}

float AKRVGameState::GetSecondsAfterFirstWaveCalling()
{
	if (GetCurrentWave() < 1)
	{
		return 0.f;
	}
	else if (GetGameStateEnum() == EGameState::Cleared || GetGameStateEnum() == EGameState::Defeated)
	{
		KRVCHECK(SecondsAtWaveCalled.Num() > 1);
		return SecondsAtWaveCalled.Last() - SecondsAtWaveCalled[0];
	}
	else
	{
		KRVCHECK(SecondsAtWaveCalled.Num() > 0);
		return UKismetSystemLibrary::GetGameTimeInSeconds(this) - SecondsAtWaveCalled[0];
	}

}

void AKRVGameState::SetGameStateEnum(EGameState NewState)
{
	InGameState = NewState;
	OnInGameStateChangeDelegate.Broadcast(NewState);

}

void AKRVGameState::SetTotalWaves(int32 NewWaves)
{
	TotalWaves = NewWaves;
	OnWaveChangedDelegate.Broadcast(CurrentWave, TotalWaves);

}

bool AKRVGameState::IsLastWave() const
{
	if (CurrentWave >= TotalWaves)
	{
		return true;
	}
	return false;

}

