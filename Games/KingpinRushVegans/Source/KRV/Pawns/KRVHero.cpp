// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVHero.h"
#include "Runtime/Engine/Classes/Engine/EngineTypes.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Sound/SoundCue.h"
#include "NavigationSystem.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/AudioComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StreamableManager.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Runtime/Engine/Classes/GameFramework/CharacterMovementComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"

#include "KRVBlueprintFunctionLibrary.h"

#include "DataAssets/KRVDataAsset.h"
#include "DataAssets/KRVCharacterDataAsset.h"
#include "DataAssets/KRVHeroDataAsset.h"

#include "Controllers/KRVInGamePlayerController.h"
#include "Controllers/KRVPlayerAIController.h"
#include "SceneComponents/SpawnerComponent.h"
#include "SceneComponents/PooledComponent.h"
#include "ActorComponents/KRVCharacterStatComponent.h"
#include "Actors/KRVSkill.h"

#include "GameModes/KRVInGameGameMode.h"
#include "PlayerStates/KRVInGamePlayerState.h"

AKRVHero::AKRVHero()
{
	// The pooled component will be destroyed if a spawner component is not set. 
	// Set Spawner component not to destroy this hero and revive. Spawner component does nothing.
	// 스포너 컴퍼넌트는 아무것도 하지 않음. 풀드컴포넌트 내 스포터컴포넌트 포인터가 없으면 BeginDestroy 
	// 함수를 부르기 때문에, 더미 컴포넌트를 생성함.
	SpawnerComponent = CreateDefaultSubobject<USpawnerComponent>(TEXT("SPAWNER_COMPONENT"));
	SpawnerComponent->SetupAttachment(RootComponent);
	PooledComponent->SetSpawnerComponent(SpawnerComponent);

	static ConstructorHelpers::FObjectFinder<UParticleSystem> ON_SPAWNED_PARTICLE_OBJ
	(TEXT("/Game/InfinityBladeEffects/Effects/FX_Combat_Base/Resurrection/P_Resurrection_02.P_Resurrection_02"));
	if (ON_SPAWNED_PARTICLE_OBJ.Succeeded())
	{
		PooledComponent->OnSpawnedParticleComponent->SetTemplate(ON_SPAWNED_PARTICLE_OBJ.Object);
		PooledComponent->OnSpawnedParticleComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, OverHeadSocket);
	}
	static ConstructorHelpers::FObjectFinder<USoundCue> ON_SPAWNED_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Magic/Cues/retro_magic_spell_cast_03_Cue.retro_magic_spell_cast_03_Cue"));
	if (ON_SPAWNED_SOUND_OBJ.Succeeded())
	{
		PooledComponent->OnSpawnedAudioComponent->SetSound(ON_SPAWNED_SOUND_OBJ.Object);
	}

	LevelUpParticleComponent = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("LEVELUP_PARTICLE_COMPONENT"));
	LevelUpParticleComponent->SetupAttachment(GetMesh(), OverHeadSocket);
	LevelUpParticleComponent->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<UParticleSystem> LEVELUP_PARTICLE_OBJ
	(TEXT("/Game/Resources/Particles/P_levelUp_1.P_levelUp_1"));
	if (LEVELUP_PARTICLE_OBJ.Succeeded())
	{
		LevelUpParticleComponent->SetTemplate(LEVELUP_PARTICLE_OBJ.Object);
	}

	LevelUpAudioComponent = CreateDefaultSubobject<UAudioComponent>(TEXT("LEVELUP_AUDIO_COMPONENT"));
	LevelUpAudioComponent->SetupAttachment(GetMesh());
	LevelUpAudioComponent->bAutoActivate = false;
	static ConstructorHelpers::FObjectFinder<USoundCue> LEVELUP_SOUND_OBJ
	(TEXT("/Game/Retro_8Bit_Sounds/Powerup/Cues/retro_powerup_collect_08_Cue.retro_powerup_collect_08_Cue"));
	if (LEVELUP_SOUND_OBJ.Succeeded())
	{
		LevelUpAudioComponent->SetSound(LEVELUP_SOUND_OBJ.Object);
	}

}

void AKRVHero::PostInitializeComponents()
{
	// LoadBaseStat call in parent class will be failed since Hero level is not informed.
	// LoadBaseStat fcn is called again in BeginPlay.
	// 부모클래스에서 부르는 로드베이스스탯 함수는 실패함. 히어로레벨 정보가 없기 때문. 비긴플레이에서 다시 불림.
	Super::PostInitializeComponents();

}

void AKRVHero::BeginPlay()
{
	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
		KRVCHECK(KRVInGamePlayerState);
		if (KRVInGamePlayerState)
		{
			UpdateGameStateExpData(KRVInGamePlayerState->GetHeroLevel());
			LoadBaseStat();
			KRVInGamePlayerState->OnLevelUpDelegate.AddUObject(this, &AKRVHero::OnLevelUp);
		}
	}

	Super::BeginPlay();

}

UKRVHeroDataAsset* AKRVHero::GetHeroDataAsset() const
{
	return Cast<UKRVHeroDataAsset>(GetDataAsset_Implementation());

}

bool AKRVHero::LoadSkillMasteryLevels()
{
	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
		if (Skills.Num() == 0 || !KRVInGamePlayerState)
		{
			KRVCHECK(Skills.Num() > 0);
			KRVCHECK(KRVInGamePlayerState);
			return false;
		}
		else
		{
			TArray<FKRVSkillMasterySaveData> SkillMasterySaveData;
			KRVInGamePlayerState->GetHeroSkillMasteryLevels(SkillMasterySaveData);
			return SetSkillLevels(SkillMasterySaveData);
		}
	}
	return false;

}

bool AKRVHero::LoadBaseStat()
{
	KRVCHECK(GetWorld());
	if (GetWorld())
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
		if (!GetDataAsset_Implementation() || !KRVInGamePlayerState)
		{
			KRVCHECK(GetDataAsset_Implementation());
			KRVCHECK(KRVInGamePlayerState);
			return false;
		}
		else
		{
			int32 LoadedHeroLevel = KRVInGamePlayerState->GetHeroLevel();
			if (LoadedHeroLevel <= 0)
			{
				KRVCHECK(LoadedHeroLevel > 0);
				return false;
			}
			else
			{
				// FName suffix means level.
				// FName 접미사는 히어로의 레벨을 의미함.
				FName RowName = FName(*((GetDataAsset_Implementation()->ClassFName).ToString() +
					FString(TEXT("_")) + FString::FromInt(LoadedHeroLevel)));
				if (GetBaseCharacterStat()->LoadData(RowName) &&
					GetModifiedCharacterStat()->LoadData(RowName))
				{
					return true;
				}
			}
		}
	}
	return false;

}

bool AKRVHero::UpdateGameStateExpData(int32 NewLevel)
{
	if (GetWorld())
	{
		AKRVInGamePlayerState* KRVInGamePlayerState = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(GetWorld());
		if (!KRVInGamePlayerState || !GetHeroDataAsset())
		{
			KRVCHECK(KRVInGamePlayerState);
			KRVCHECK(GetHeroDataAsset());
			return false;
		}
		else
		{
			if (GetHeroDataAsset()->ExpToLevelUp.IsValidIndex(NewLevel - 1))
			{
				KRVInGamePlayerState->SetExpToLevelUp(GetHeroDataAsset()->ExpToLevelUp[NewLevel - 1]);
			}
			else // Max Level reached or InValidLevel;
			{
				KRVInGamePlayerState->SetExpToLevelUp(-1);
			}
			return true;
		}
	}
	return false;

}

void AKRVHero::OnLevelUp(int32 NewLevel)
{
	UpdateGameStateExpData(NewLevel);
	LoadBaseStat();
	float LastMaxHealth = GetBaseCharacterStat()->DefenceStat.MaxHealth;
	GetModifiedCharacterStat()->GainHealth(FMath::Abs(GetBaseCharacterStat()->DefenceStat.MaxHealth - LastMaxHealth));
	LevelUpParticleComponent->ActivateSystem();
	LevelUpAudioComponent->Play();

}

bool AKRVHero::Die()
{
	if (Super::Die() && GetWorld())
	{
		GetWorldTimerManager().SetTimer(ReviveTimer, this, &AKRVHero::Revive,
			GetHeroDataAsset()->ReviveTime, false);
		ReviveTimeNotificationDelegate.Broadcast(GetRemainingReviveTime(), GetHeroDataAsset()->ReviveTime);
		return true;
	}
	return false;

}

void AKRVHero::Revive()
{
	PooledComponent->SetNearActorActive(true);
	ReviveTimeNotificationDelegate.Broadcast(GetRemainingReviveTime(), GetHeroDataAsset()->ReviveTime);
	BeInvincible(InvincibleTimeWhenRevive);

}

float AKRVHero::GetRemainingReviveTime()
{
	if (GetWorld())
	{
		return GetWorldTimerManager().GetTimerRemaining(ReviveTimer);
	}
	else
	{
		return -1.f;
	}

}

