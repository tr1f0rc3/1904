// Fill out your copyright notice in the Description page of Project Settings.

#include "GameModes/KRVInGameGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Controller.h"
#include "Engine/AssetManager.h"
#include "Engine/StreamableManager.h"

#include "KRV.h"
#include "KRVEnums.h"
#include "KRVBlueprintFunctionLibrary.h"

#include "DataAssets/KRVDataAsset.h"
#include "DataAssets/KRVLevelDataAsset.h"

#include "ActorComponents/KRVCharacterStatComponent.h"
#include "KRVGameInstance.h"
#include "KRVGameState.h"
#include "PlayerStates/KRVInGamePlayerState.h"
#include "Actors/KRVGateway.h"
#include "Actors/KRVSkill.h"
#include "Actors/KRVItem.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Pawns/KRVTower.h"
#include "Pawns/KRVPlayerCameraPawn.h"
#include "Controllers/KRVAIController.h"
#include "Controllers/KRVInGamePlayerController.h"
#include "Controllers/KRVPlayerAIController.h"
#include "Controllers/KRVEnemyAIController.h"

AKRVInGameGameMode::AKRVInGameGameMode()
{
	DefaultPawnClass = AKRVPlayerCameraPawn::StaticClass();
	PlayerControllerClass = AKRVInGamePlayerController::StaticClass();
	PlayerStateClass = AKRVInGamePlayerState::StaticClass();
	GameStateClass = AKRVGameState::StaticClass();

}

void AKRVInGameGameMode::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	LoadFromDataAsset_Implementation();

}

void AKRVInGameGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	auto KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPlayer->PlayerState);
	KRVCHECK(KRVInGamePlayerState);
	if (KRVInGamePlayerState)
	{
		KRVInGamePlayerState->LoadInGameSave();
	}

}

UKRVDataAsset* AKRVInGameGameMode::GetDataAsset_Implementation() const
{
	return Cast<UKRVDataAsset>(LevelDataAsset);

}

bool AKRVInGameGameMode::LoadFromDataAsset_Implementation()
{
	UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(this);
	if (!KRVGameInstance)
	{
		KRVCHECK(KRVGameInstance);
	}
	else
	{
		LevelDataAsset = KRVGameInstance->GetLevelDataAsset(UGameplayStatics::GetCurrentLevelName(this, true));
		if (!LevelDataAsset)
		{
			KRVCHECK(LevelDataAsset);
		}
		else
		{
			GetKRVGameState()->SetHearts(LevelDataAsset->GivenHearts);
			return true;
		}
	}
	return false;

}


UKRVLevelDataAsset* AKRVInGameGameMode::GetLevelDataAsset() const
{
	return LevelDataAsset;

}

AKRVGameState* AKRVInGameGameMode::GetKRVGameState()
{
	if (!KRVGameState && GetWorld())
	{
		KRVGameState = UKRVBlueprintFunctionLibrary::GetKRVGameState(GetWorld());
		KRVCHECK(KRVGameState);
	}
	return KRVGameState;

}


int32 AKRVInGameGameMode::GetCurrentWave()
{
	if (GetKRVGameState())
	{
		return GetKRVGameState()->GetCurrentWave();
	}
	return -1;

}

void AKRVInGameGameMode::CallIncoming(float CallTimeLimit /*= -1.f*/)
{
	GetKRVGameState()->SetGameStateEnum(EGameState::Incoming);
	OnNextWaveIncomingDelegate.Broadcast(CallTimeLimit);
	bIsNextWaveIncoming = true;

	if (GetWorld() && CallTimeLimit > 0.f)
	{
		GetWorldTimerManager().SetTimer(NextWaveIncomingTimer, this, &AKRVInGameGameMode::CallNextWave, CallTimeLimit, false);
	}

}

void AKRVInGameGameMode::CallNextWave()
{
	if (bIsNextWaveIncoming)
	{
		bool bIsCallingNextWavePermitted = false;
		if (GetKRVGameState())
		{
			EGameState CurrentState = KRVGameState->GetGameStateEnum();
			switch (GetKRVGameState()->GetGameStateEnum())
			{
			case EGameState::Preparing:
			{
				bIsCallingNextWavePermitted = true;
				break;
			}
			case EGameState::Incoming:
			{
				bIsCallingNextWavePermitted = true;
				break;
			}
			default:
				break;
			}
		}

		if (bIsCallingNextWavePermitted)
		{
			GetKRVGameState()->SetGameStateEnum(EGameState::Spawning);
			GetKRVGameState()->SecondsAtWaveCalled.Emplace(UKismetSystemLibrary::GetGameTimeInSeconds(this));
			GetKRVGameState()->JumpToNextWave(1);
			OnNextWaveCalledDeleagte.Broadcast();
		}
		bIsNextWaveIncoming = false;
	}

}

void AKRVInGameGameMode::EndGame(EKRVTeam NewWinner)
{
	if (!GetKRVGameState() || !GetWorld())
	{
		KRVCHECK(GetKRVGameState());
		KRVCHECK(GetWorld());
	}
	else
	{
		AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld());
		if (!GetLevelDataAsset() || !KRVInGamePlayerController)
		{
			KRVCHECK(GetLevelDataAsset());
			KRVCHECK(KRVInGamePlayerController);
		}
		else
		{
			switch (NewWinner)
			{
			case EKRVTeam::Player:
			{
				GetKRVGameState()->SetGameStateEnum(EGameState::Cleared);
				int32 StarsToEarnInThisLevel = GetStarsToEarn();
				GetKRVGameState()->EarnRuby(KRVInGamePlayerController, GetLevelDataAsset()->RubyToEarn);
				GetKRVGameState()->SetGameResultOfPlayerState(KRVInGamePlayerController, NewWinner, StarsToEarnInThisLevel);
				//GetKRVGameState()->UnlockRewardsAndSaveGame(KRVInGamePlayerController, LevelDataAsset);
				break;
			}
			case EKRVTeam::Enemy:
			{
				GetKRVGameState()->SetGameStateEnum(EGameState::Defeated);
				GetKRVGameState()->SetGameResultOfPlayerState(KRVInGamePlayerController, NewWinner);
				break;
			}
			default:
				break;
			}
		}

		if (GetWorld())
		{
			GetWorldTimerManager().SetTimer(GameStopTimerOnGameEnd, this, &AKRVInGameGameMode::StopGameAfterGameEnd, GameStopDelayAfterGameEnd, false);
		}
		OnGameEndDelegate.Broadcast(NewWinner);
	}

}

int32 AKRVInGameGameMode::GetStarsToEarn()
{
	int32 StarsToEarnInThisLevel = 0;
	if (!GetKRVGameState() || !GetLevelDataAsset())
	{
		KRVCHECK(GetKRVGameState());
		KRVCHECK(GetLevelDataAsset());
	}
	else if (GetKRVGameState()->GetGameStateEnum() == EGameState::Cleared)
	{
		StarsToEarnInThisLevel = 1;
		for (const FKRVLevelStarQualification& LevelStarQual : GetLevelDataAsset()->LevelStarQualifications)
		{
			if (LevelStarQual.StarsToEarn > StarsToEarnInThisLevel)
			{
				bool bIsClearTimeQualified = LevelStarQual.ClearTime < 0 ||
					GetKRVGameState()->GetSecondsAfterFirstWaveCalling() <= LevelStarQual.ClearTime;
				bool bIsRemainingHeartsQualified = GetKRVGameState()->GetHeartsRemaining() >=
					LevelStarQual.HeartsRemaining;
				if (bIsClearTimeQualified && bIsRemainingHeartsQualified)
				{
					StarsToEarnInThisLevel = LevelStarQual.StarsToEarn;
				}
			}
		}
	}
	return StarsToEarnInThisLevel;

}

//bool AKRVInGameGameMode::UnlockRewards(AKRVInGamePlayerController* NewPC)
//{
//	if (!GetLevelDataAsset() || !NewPC)
//	{
//		KRVCHECK(GetLevelDataAsset());
//		KRVCHECK(NewPC);
//	}
//	else
//	{
//		AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewPC->PlayerState);
//		if (!KRVInGamePlayerState)
//		{
//			KRVCHECK(KRVInGamePlayerState);
//		}
//		else
//		{
//			KRVInGamePlayerState->UnlockRewardsAndSaveGame(GetLevelDataAsset());
//		}
//	}
//	return false;
//
//}

void AKRVInGameGameMode::ShowNextWavePaths(bool bIsTrue)
{
	OnNextWavePathsShownDelegate.Broadcast(bIsTrue);

}

void AKRVInGameGameMode::StopGameAfterGameEnd()
{
	if (GetWorld())
	{
		UGameplayStatics::SetGamePaused(GetWorld(), true);
	}
	OnGameStoppedAfterGameEndDelegate.Broadcast();

}


void AKRVInGameGameMode::OnTheGateFinishSpawnInTheWave(float CallTimeLimit, class AKRVGateway* TheGateWay)
{
	if (++GatesSpawnInTheWaveFinished == Gateways.Num())
	{
		GatesSpawnInTheWaveFinished = 0;
		if (GetKRVGameState() && !GetKRVGameState()->IsLastWave())
		{
			CallIncoming(CallTimeLimit);
			//OnAllGatesFinishSpawnInTheWaveDeleagte.Broadcast(CallTimeLimit);
		}
	}

}

void AKRVInGameGameMode::OnTheGateFinishAllSpawn(AKRVGateway* TheGateWay)
{
	if (++GatesAllSpawnFinished == Gateways.Num())
	{
		if (GetKRVGameState())
		{
			GetKRVGameState()->SetGameStateEnum(EGameState::AllSpawnFinished);
		}
		OnAllGatesFinishAllSpawnDeleagte.Broadcast();
	}

}

void AKRVInGameGameMode::AddGate(AKRVGateway* NewGateWay)
{
	KRVCHECK(NewGateWay);
	if (NewGateWay)
	{
		Gateways.Emplace(NewGateWay);

		if (GetKRVGameState())
		{
			KRVCHECK(!(GetKRVGameState()->GetTotalWaves() != 0 &&
				GetKRVGameState()->GetTotalWaves() != NewGateWay->GetTotalWaves()));
			GetKRVGameState()->SetTotalWaves(FMath::Max3(GetKRVGameState()->GetTotalWaves(), NewGateWay->GetTotalWaves(), 0));
		}
	}

}

int32 AKRVInGameGameMode::GetRemainingStock(class AKRVItem* ItemToGet, class AKRVCombatUnit* NewCaster)
{
	if (!ItemToGet || !NewCaster)
	{
		KRVCHECK(ItemToGet);
		KRVCHECK(NewCaster);
	}
	else
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = Cast<AKRVInGamePlayerState>(NewCaster->GetPlayerState<AKRVInGamePlayerState>());
		if (!KRVInGamePlayerState)
		{
			KRVCHECK(KRVInGamePlayerState);
		}
		else
		{
			return KRVInGamePlayerState->GetRemainingItemStock(ItemToGet);
		}
	}
	return false;

}

int32 AKRVInGameGameMode::UseItem(AKRVItem* ItemToUse, AKRVCombatUnit* NewCaster)
{
	if (!ItemToUse || !NewCaster)
	{
		KRVCHECK(ItemToUse);
		KRVCHECK(NewCaster);
	}
	else
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = NewCaster->GetPlayerState<AKRVInGamePlayerState>();
		if (!KRVInGamePlayerState)
		{
			KRVCHECK(KRVInGamePlayerState);
		}
		else
		{
			return KRVInGamePlayerState->UseItem(ItemToUse);
		}
	}
	return -1;

}

int32 AKRVInGameGameMode::IssueUnitInFieldID()
{
	return ++RecentlyIssuedUnitInFieldID;

}

bool AKRVInGameGameMode::IsKiiledByPlayer(class AKRVCharacter* DeadCharacter)
{
	if (!DeadCharacter)
	{
		KRVCHECK(DeadCharacter);
		return false;
	}
	else if (DeadCharacter->GetActionStatus() != EActionStatus::Dead)
	{
		return false;
	}
	else
	{
		// Get Killer Controller
		AKRVInGamePlayerController* KillerPlayerCtrl = Cast<AKRVInGamePlayerController>(DeadCharacter->LastHitBy);
		AKRVAIController* KillerAICtrl = Cast<AKRVAIController>(DeadCharacter->LastHitBy);
		if (!KillerPlayerCtrl)
		{
			if (KillerAICtrl)
			{
				KillerPlayerCtrl = KillerAICtrl->GetPlayerControllerByTeam();
				if (!KillerPlayerCtrl)
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}

		if (KillerPlayerCtrl)
		{
			// Get KillerPawn
			AKRVCombatUnit* Killer = Cast<AKRVCombatUnit>(KillerAICtrl->GetPawn());
			if (!Killer)
			{
				return false;
			}
			else if (DeadCharacter->IsHostile(Killer))
			{
				return true;
			}
		}
	}
	return false;

}

void AKRVInGameGameMode::OnEnemySpawned(AKRVCharacter* SpwanedCharacter)
{
	if (!GetKRVGameState() || !SpwanedCharacter)
	{
		KRVCHECK(GetKRVGameState());
		KRVCHECK(SpwanedCharacter);
	}
	else if (SpwanedCharacter->GetTeam() == EKRVTeam::Enemy)
	{
		KRVCHECK(VariateEnemiesInField(1));
	}

}

bool AKRVInGameGameMode::VariateEnemiesInField(int32 EnemiesToAdd)
{
	if (!GetKRVGameState())
	{
		KRVCHECK(GetKRVGameState());
	}
	else
	{
		GetKRVGameState()->EnemiesInField += EnemiesToAdd;
		if (GetKRVGameState()->EnemiesInField <= 0)
		{
			switch (GetKRVGameState()->GetGameStateEnum())
			{
			case EGameState::Preparing:
			{
				KRVLOG(Error, TEXT("Character is not supposed to die in preapareing state"));
				break;
			}
			case EGameState::Spawning:
			{
				KRVLOG(Error, TEXT("Character is not supposed to die in spawning state"));
				break;
			}
			case EGameState::AllSpawnFinished:
			{
				EndGame(EKRVTeam::Player);
				break;
			}
			}
		}
		return true;
	}
	return false;

}

void AKRVInGameGameMode::OnRecruited(AKRVCharacter* SpawnedCharacter)
{
	if (!GetKRVGameState() || !SpawnedCharacter)
	{
		KRVCHECK(GetKRVGameState());
		KRVCHECK(SpawnedCharacter);
	}
	else if (SpawnedCharacter->GetTeam() == EKRVTeam::Player)
	{
		AKRVAIController* KRVAIController = SpawnedCharacter->GetPossessingKRVAIController();
		if (KRVAIController)
		{
			GetKRVGameState()->AddRecruits(KRVAIController->GetPlayerControllerByTeam(), 1);
		}
	}

}

void AKRVInGameGameMode::OnUnitDie(class AKRVCombatUnit* DeadUnit)
{
	if (!DeadUnit)
	{
		KRVCHECK(DeadUnit);
	}
	else
	{
		bool bIsRewardGained = false;

		switch (DeadUnit->GetTeam())
		{
		case EKRVTeam::Player:
		{
			// not implemented yet
			break;
		}
		case EKRVTeam::Enemy:
		{
			AKRVCharacter* DeadEnemy = Cast<AKRVCharacter>(DeadUnit);
			if (GetKRVGameState() && DeadEnemy)
			{
				KRVCHECK(VariateEnemiesInField(-1));
			}
			break;
		}
		default:
			break;
		}
	}

}

void AKRVInGameGameMode::OnUnitDieWithReward(int32 NewGold, int32 NewExp, int32 NewRuby, class AKRVCombatUnit* DeadUnit)
{
	if (!DeadUnit)
	{
		KRVCHECK(DeadUnit);
	}
	else
	{
		bool bIsRewardGained = false;

		switch (DeadUnit->GetTeam())
		{
		case EKRVTeam::Player:
		{
			// not implemented yet
			break;
		}
		case EKRVTeam::Enemy:
		{
			AKRVCharacter* DeadCharacter = Cast<AKRVCharacter>(DeadUnit);
			if (IsKiiledByPlayer(DeadCharacter))
			{
				// Add Kills and golds and exp
				if (GetKRVGameState() && GetWorld())
				{
					GetKRVGameState()->GainKillRewards(UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(GetWorld()), NewGold, NewExp, NewRuby);
					bIsRewardGained = true;
				}
			}
			break;
		}
		default:
			break;
		}

		// Play Popup Effect;
		if (bIsRewardGained)
		{
			if (NewGold > 0)
			{
				DeadUnit->PlayGoldPopUpEffect();
			}
			if (NewRuby > 0)
			{
				DeadUnit->PlayRubyPopUpEffect();
			}
		}
	}


}

void AKRVInGameGameMode::OnEnemyEscape(class AKRVCharacter* EscapedEnemy)
{
	KRVCHECK(EscapedEnemy);
	if (EscapedEnemy->GetTeam() != EKRVTeam::Enemy)
	{
		KRVCHECK(EscapedEnemy->GetTeam() == EKRVTeam::Enemy);
		return;
	}

	int32 HeartsTaken = EscapedEnemy->GetBaseCharacterStat()->LivesTaken;
	EscapedEnemy->OnEscaped();
	VariateEnemiesInField(-1);

	if (GetKRVGameState())
	{
		GetKRVGameState()->AddEscapedEnemies(1);
		int32 HeartsRemaining = GetKRVGameState()->AddHearts(-HeartsTaken);
		if (HeartsRemaining <= 0)
		{
			GetKRVGameState()->SetGameStateEnum(EGameState::Defeated);
			EndGame(EKRVTeam::Enemy);
		}
	}

}

void AKRVInGameGameMode::OnTowerSold(int32 Price, class AKRVTower* SoldTower)
{
	AddAvailableGold(Price);
	if (SoldTower)
	{
		SoldTower->PlayGoldPopUpEffect();
	}
}

void AKRVInGameGameMode::AddHearts(int32 _Hearts)
{
	if (GetKRVGameState())
	{
		GetKRVGameState()->AddHearts(_Hearts);
	}

}

void AKRVInGameGameMode::AddAvailableGold(int32 NewGold)
{
	if (GetKRVGameState())
	{
		GetKRVGameState()->AddGold(NewGold);
	}

}

float AKRVInGameGameMode::ApplyDamageAndPlayEffect(const FHitResult& SweepResult, float BaseDamage, AKRVCombatUnit* DamageCauser, TSubclassOf<UKRVDamageType> DamageTypeClass)
{
	AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(SweepResult.GetActor());
	if (KRVCharacter)
	{
		float AppliedDamage = ApplyDamage(KRVCharacter, BaseDamage, DamageCauser, DamageTypeClass);
		PlayDamageTypeEffect(SweepResult, DamageTypeClass);
		return AppliedDamage;
	}
	else
	{
		KRVCHECK(KRVCharacter);
		return -1.f;
	}

}

bool AKRVInGameGameMode::ApplyRadialDamageAndPlayEffect(float MinDamage, float MaxDamage, const FVector& DamageOrigin, float DamageRadius, AActor* DamageCauser, TSubclassOf<UKRVDamageType> DamageTypeClass, bool bDoesDrawDebugLine /*= false*/)
{
	if (!DamageCauser || !GetWorld())
	{
		KRVCHECK(DamageCauser);
		KRVCHECK(GetWorld());
		return false;
	}
	else
	{

		//UKRVBlueprintFunctionLibrary::MultiRadialTrace_Template(const AKRVCombatUnit * Lhs, TArray<T*> & OutActors,
		//	const FVector & StartLoc, float Radius, ETargetType TargetType = ETargetType::AnyPawn, bool bDoesDrawDebugLine = false)



		TArray<FHitResult> DamageHits;
		UKRVBlueprintFunctionLibrary::MultiRadialPawnTrace(GetWorld(), DamageHits, DamageOrigin, DamageRadius, false);
		AController* DamageInstigator = nullptr;
		AKRVCombatUnit* DamageCausingUnit = Cast<AKRVCombatUnit>(DamageCauser);
		if (DamageCausingUnit)
		{
			DamageInstigator = DamageCausingUnit->GetController();
		}

		for (int32 Index = DamageHits.Num() - 1; Index >= 0; --Index)
		{
			AKRVCharacter* HitCharacterToCheck = Cast<AKRVCharacter>(DamageHits[Index].GetActor());
			if (HitCharacterToCheck)
			{
				if (DamageCausingUnit)
				{
					if (UKRVBlueprintFunctionLibrary::IsHostile(DamageCausingUnit, Cast<AKRVCombatUnit>(HitCharacterToCheck)))
					{
						continue;
					}
				}
				else
				{
					continue;
				}
			}
			DamageHits.RemoveAt(Index, 1, false);
		}
		DamageHits.Shrink();

		for (const FHitResult& DamageHit : DamageHits)
		{
			UGameplayStatics::ApplyDamage(DamageHit.GetActor(), UKismetMathLibrary::RandomFloatInRange(MinDamage, MaxDamage),
				DamageInstigator, DamageCauser, DamageTypeClass);
			PlayDamageTypeEffect(DamageHit, DamageTypeClass);
		}

		if (DamageHits.Num() > 0)
		{
			return true;
		}
	}
	return false;

}

float AKRVInGameGameMode::ApplyDamage(AKRVCharacter* DamagedActor, float BaseDamage, AKRVCombatUnit* DamageCauser, TSubclassOf<UKRVDamageType> DamageTypeClass)
{
	float AppliedDamage = UGameplayStatics::ApplyDamage(Cast<AActor>(DamagedActor), BaseDamage,
		DamageCauser->GetController(), Cast<AActor>(DamageCauser), DamageTypeClass);
	return AppliedDamage;

}

void AKRVInGameGameMode::PlayDamageTypeEffect(const FHitResult& _HitResult, TSubclassOf<UKRVDamageType> DamageTypeClass) const
{
	UWorld* World = GetWorld();
	KRVCHECK(World);
	if (World)
	{
		FVector PlayLocation = _HitResult.Location;
		FRotator PlayRotation = UKismetMathLibrary::MakeRotFromX(_HitResult.Normal);
		UKRVDamageType* KRVDamageType = Cast<UKRVDamageType>(DamageTypeClass->GetDefaultObject());
		EDamageEffectType DamageEffectType = KRVDamageType->DamageEffectType;

		UKRVGameInstance* const KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(World);
		KRVCHECK(KRVGameInstance);
		if (KRVGameInstance)
		{
			UParticleSystem* DamageTypeEffectParticle = KRVGameInstance->GetDamageEffectTypeParticle(DamageEffectType);
			if (DamageTypeEffectParticle)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, DamageTypeEffectParticle,
					PlayLocation, PlayRotation, true);
			}
			USoundCue* DamageTypeEffectSound = KRVGameInstance->GetDamageEffectTypeSound(DamageEffectType);
			if (DamageTypeEffectSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, DamageTypeEffectSound, PlayLocation, 1.f);
			}
		}
	}

}

