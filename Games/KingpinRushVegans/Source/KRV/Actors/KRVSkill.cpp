// Fill out your copyright notice in the Description page of Project Settings.


#include "KRVSkill.h"
#include "TimerManager.h"
#include "Engine/AssetManager.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/BoxComponent.h"

#include "KRV.h"
#include "KRVBlueprintFunctionLibrary.h"
#include "DataAssets/KRVShooterComponentDataAsset.h"
#include "SceneComponents/KRVShooterComponent.h"
#include "SceneComponents/SpawnerComponent.h"
#include "SceneComponents/PooledComponent.h"
#include "Actors/KRVBuff.h"
#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "KRVGameState.h"
#include "GameModes/KRVInGameGameMode.h"

AKRVSkill::AKRVSkill()
{
	PrimaryActorTick.bCanEverTick = false;

	PooledComponent = CreateDefaultSubobject<UPooledComponent>(TEXT("POOLED_COMPONENT"));
	PooledComponent->RegisterComponent();

	SetMasteryLevel(MasteryLevel);
	bIsDataLoaded = false;
	bIsInCooldown = false;

}

void AKRVSkill::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	GetDataAsset();
	LoadFromDataAsset_Implementation();

}

UKRVDataAsset* AKRVSkill::GetDataAsset_Implementation() const
{
	return HardPtr_MyDataAsset;

}

bool AKRVSkill::LoadFromDataAsset_Implementation()
{
	FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
	HardPtr_MyDataAsset = Cast<UKRVDataAsset>(Streamable.LoadSynchronous(MyDataAsset.ToSoftObjectPath(), true));
	if (!GetDataAsset_Implementation() || bIsDataLoaded)
	{
		KRVCHECK(!bIsDataLoaded);
		KRVCHECK(GetDataAsset_Implementation());
		return false;
	}

	AKRVCombatUnit* KRVCombatUnit = GetKRVOwner();
	if (!KRVCombatUnit)
	{
		KRVCHECK(KRVCombatUnit);
		return false;
	}
	else
	{
		// Projectile setup
		for (int32 Index = 0; Index < MyShooterComponentDataAsset.Num(); ++Index)
		{
			HardPtr_ShooterComponentDataAsset.Emplace
			(Cast<UKRVShooterComponentDataAsset>
				(Streamable.LoadSynchronous(MyShooterComponentDataAsset[Index].ToSoftObjectPath(), true)));
			UKRVShooterComponent* ShooterComponent = NewObject<UKRVShooterComponent>(this);
			ShooterComponent->RegisterComponent();
			ShooterComponent->SetDataAssetAndLoad(HardPtr_ShooterComponentDataAsset[Index]);
			ShooterComponent->SetShooter(KRVCombatUnit);
			ShooterComponent->AttachToComponent
			(KRVCombatUnit->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale
				, RightHandSocket);
			//ShooterComponent->AddRelativeRotation(FRotator(0.f, 0.f, 0.f));
			ShooterComponents.Emplace(ShooterComponent);
		}

		// Summon skill setup
		if (CharacterSpawnData.Num() > 0)
		{
			for (int32 Index = 0; Index < CharacterSpawnData.Num(); ++Index)
			{
				USpawnerComponent* SpawnerComponent = NewObject<USpawnerComponent>(this);
				SpawnerComponent->RegisterComponent();
				SpawnerComponent->SetPoolWithCharacterSpawnData(CharacterSpawnData[Index]);
				SpawnerComponent->AttachToComponent
				(KRVCombatUnit->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				SpawnerComponents.Emplace(SpawnerComponent);
			}
		}

		// Buff skill setup
		if (BuffClasses.Num() > 0)
		{
			for (int32 Index = 0; Index < BuffClasses.Num(); ++Index)
			{
				USpawnerComponent* SpawnerComponent = NewObject<USpawnerComponent>(this);
				SpawnerComponent->RegisterComponent();
				SpawnerComponent->SetPool(BuffClasses[Index], MaxBuffs);
				SpawnerComponent->AttachToComponent
				(KRVCombatUnit->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale);
				SpawnerComponents.Emplace(SpawnerComponent);
			}
		}

		bIsDataLoaded = true;
		return true;
	}
	return false;

}

bool AKRVSkill::CastToSelf_Implementation(AKRVCombatUnit* SkillCaster)
{
	KRVCHECK(SkillCaster);
	if (SkillCaster)
	{
		OnCastSkillToSelfDelegate.Broadcast(SkillCaster);
	}
	OnCastSkill_General(SkillCaster);
	return true;

}

bool AKRVSkill::CastToTarget_Implementation(AKRVCombatUnit* Target, AKRVCombatUnit* SkillCaster)
{
	KRVCHECK(SkillCaster);
	if (SkillCaster)
	{
		OnCastSkillToTargetDelegate.Broadcast(Target, SkillCaster);
	}
	OnCastSkill_General(SkillCaster);
	return true;

}

bool AKRVSkill::CastToLocation_Implementation(const FVector& Location, AKRVCombatUnit* SkillCaster)
{
	KRVCHECK(SkillCaster);
	if (SkillCaster)
	{
		OnCastSkillToLocationDelegate.Broadcast(Location, SkillCaster);
	}
	OnCastSkill_General(SkillCaster);
	return true;

}

ETargetingType AKRVSkill::GetTargetingType() const
{
	return WhereToCastSkill;

}

ETargetType AKRVSkill::GetTargetType() const
{
	return TargetType;

}

float AKRVSkill::GetSkillRange() const
{
	return SkillRange;

}

float AKRVSkill::GetSkillRadius() const
{
	return SkillRadius;

}

bool AKRVSkill::IsAutoCastable() const
{
	return bIsAutoCasting;

}

bool AKRVSkill::GetDescriptionWithVaraibles_Implementation(FText& OutText)
{
	if (!GetDataAsset_Implementation())
	{
		KRVCHECK(GetDataAsset_Implementation());
	}
	else if (GetDataAsset_Implementation()->Description.IsEmptyOrWhitespace())
	{
		KRVCHECK(!GetDataAsset_Implementation()->Description.IsEmptyOrWhitespace())
	}
	else
	{
		SetValuesByLevel();
		OutText = GetDataAsset_Implementation()->Description;
		return true;
	}
	return false;

}

float AKRVSkill::GetCastingTime() const
{
	return CastingTime;

}

void AKRVSkill::SetMasteryLevel(int32 NewLevel)
{
	MasteryLevel = FMath::Clamp<int32>(NewLevel, 0, MaxMasteryLevel);

}

bool AKRVSkill::MasteryLevelUp(int32 LevelToIncrease /*= 1*/)
{
	if (MasteryLevel + LevelToIncrease > MaxMasteryLevel)
	{
		return false;
	}
	else
	{
		SetMasteryLevel(MasteryLevel + LevelToIncrease);
		return true;
	}

}

float AKRVSkill::GetValueByLevel_Float(int32 NewLevel, const TArray<float>& NewFloats)
{
	int32 NewIndex = NewLevel - 1;
	if (NewLevel == 0 && NewFloats.IsValidIndex(0))
	{
		return NewFloats[0];
	}
	else if (!NewFloats.IsValidIndex(NewIndex))
	{
		KRVCHECK(NewFloats.IsValidIndex(NewIndex));
	}
	else
	{
		return NewFloats[NewIndex];
	}
	return -1.f;

}

int32 AKRVSkill::GetValueByLevel_Int(int32 NewLevel, const TArray<int32>& NewInts)
{
	int32 NewIndex = NewLevel - 1;
	if (NewLevel == 0 && NewInts.IsValidIndex(0))
	{
		return NewInts[0];
	}
	if (!NewInts.IsValidIndex(NewIndex))
	{
		KRVCHECK(NewInts.IsValidIndex(NewIndex));
	}
	else
	{
		return NewInts[NewIndex];
	}
	return -1;

}

int32 AKRVSkill::GetMasteryLevel() const
{
	return MasteryLevel;

}

int32 AKRVSkill::GetMaxMasteryLevel() const
{
	return MaxMasteryLevel;

}

bool AKRVSkill::IsAbleToCastSkill_Implementation(AKRVCombatUnit* NewCaster /*= nullptr*/)
{
	if (!GetWorld() || !GetDataAsset_Implementation())
	{
		KRVCHECK(GetWorld());
		KRVCHECK(GetDataAsset_Implementation());
	}
	else if (!IsSkillReady() || MasteryLevel <= 0)
	{
		KRVCHECK(IsSkillReady());
		KRVCHECK(MasteryLevel > 0);
	}
	else
	{
		return true;
	}
	return false;

}


bool AKRVSkill::IsInSkillRange_Target_Implementation(const AKRVCombatUnit* Target, const AKRVCombatUnit* Caster) const
{
	if (!Target || !Caster || GetTargetingType() != ETargetingType::Target)
	{
		KRVCHECK(Target);
		KRVCHECK(Caster);
		KRVCHECK(GetTargetingType() == ETargetingType::Target);
		return false;
	}
	else
	{
		if (!UKRVBlueprintFunctionLibrary::IsSuitableTarget(Target, Caster, GetTargetType()) ||
			Target->GetActionStatus() == EActionStatus::Dead)
		{
			return false;
		}
		else if (Caster->GetHorizontalDistanceTo(Target) > GetSkillRange() ||
			!Caster->IsUnitInSight(Target))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

}

bool AKRVSkill::IsInSkillRange_Location_Implementation(const FVector& Location, const AKRVCombatUnit* Caster) const
{
	if (!Caster || GetTargetingType() != ETargetingType::Location || Location == FVector::ZeroVector)
	{
		KRVCHECK(Caster);
		KRVCHECK(GetTargetingType() == ETargetingType::Location);
		return false;
	}
	else
	{
		if (FVector::DistSquared2D(Caster->GetActorLocation(), Location) >
			FMath::Square(GetSkillRange()))
		{
			return false;
		}
		else
		{
			return true;
		}
	}

}

AKRVCombatUnit* AKRVSkill::GetKRVOwner() const
{
	return Cast<AKRVCombatUnit>(GetOwner());

}

bool AKRVSkill::SummonCharacter(int32 SpawnerCompIndex, int32 NewCharactersToSpawn, float NewLifeSpawn, AKRVCombatUnit* Caster)
{
	if (!IsValidSpawnerComponentIndex(SpawnerCompIndex))
	{
		return false;
	}
	else if (!SpawnerComponents[SpawnerCompIndex]->GetPooledActorClass()->IsChildOf(AKRVCharacter::StaticClass()))
	{
		KRVCHECK(SpawnerComponents[SpawnerCompIndex]->GetPooledActorClass()->IsChildOf(AKRVCharacter::StaticClass()));
		return false;
	}
	else
	{
		bool bReturnResult = true;
		USpawnerComponent* SpawnerComponent = SpawnerComponents[SpawnerCompIndex];
		const FKRVCharacterSpawnData& CSData = CharacterSpawnData[SpawnerCompIndex];
		SpawnerComponent->SpawnedActorLifeSpan = NewLifeSpawn;
		TArray<AActor*> Summons;
		SpawnerComponent->SetSpawnRate(NewCharactersToSpawn, CSData.SpawnRate);
		SpawnerComponent->SpawnWithOutArray(Summons);
		for (int32 Index = 0; Index < Summons.Num(); ++Index)
		{
			if (!Summons[Index])
			{
				KRVCHECK(Summons[Index]);
				bReturnResult = false;
			}
			else
			{
				AKRVCharacter* KRVCharacter = Cast<AKRVCharacter>(Summons[Index]);
				if (!KRVCharacter)
				{
					KRVCHECK(KRVCharacter);
					bReturnResult = false;
				}
				else
				{
					KRVCharacter->BePossessedByAIController(Caster->GetTeam(), true);
				}
			}
		}
		return bReturnResult;
	}

}

bool AKRVSkill::GiveBuff(TArray<AKRVCombatUnit*> TargetUnits, int32 SpawnerCompIndex, AKRVCombatUnit* Caster)
{
	if (!IsValidSpawnerComponentIndex(SpawnerCompIndex))
	{
		return false;
	}
	else if (!SpawnerComponents[SpawnerCompIndex]->GetPooledActorClass()->IsChildOf(AKRVBuff::StaticClass()))
	{
		KRVCHECK(SpawnerComponents[SpawnerCompIndex]->GetPooledActorClass()->IsChildOf(AKRVBuff::StaticClass()));
		return false;
	}
	else
	{
		USpawnerComponent* SpawnerComponent = SpawnerComponents[SpawnerCompIndex];
		bool bReturnResult = true;
		SpawnerComponent->SetSpawnParams(FVector::ZeroVector, FRotator::ZeroRotator, false,
			FMath::Min3<int32>(MaxBuffs, MaxBuffsInField, TargetUnits.Num()), 0.f);
		TArray<AActor*> Buffs;
		SpawnerComponent->SpawnWithOutArray(Buffs);
		for (int32 Index = 0; Index < Buffs.Num(); ++Index)
		{
			if (!TargetUnits[Index] || !Buffs[Index])
			{
				KRVCHECK(TargetUnits[Index]);
				KRVCHECK(Buffs[Index]);
				bReturnResult = false;
			}
			else
			{
				AKRVBuff* Buff = Cast<AKRVBuff>(Buffs[Index]);
				if (!Buff)
				{
					KRVCHECK(Buff);
				}
				else
				{
					Buff->GiveThis(TargetUnits[Index], Caster);
				}
			}
		}
		return bReturnResult;
	}

}

bool AKRVSkill::IsValidSpawnerComponentIndex(int32 Index)
{
	if (!SpawnerComponents.IsValidIndex(Index))
	{
		KRVCHECK(SpawnerComponents.IsValidIndex(Index));
		return false;
	}
	else if (!SpawnerComponents[Index])
	{
		KRVCHECK(SpawnerComponents[Index]);
		return false;
	}
	else
	{
		return true;
	}

}

void AKRVSkill::OnCastSkill_General(AKRVCombatUnit* NewSkillCaster)
{
	if (!GetWorld())
	{
		KRVCHECK(GetWorld());
	}
	else if (SkillCooldown > 0.f)
	{
		bIsInCooldown = true;
		GetWorld()->GetTimerManager().SetTimer(CooldownTimer, this, &AKRVSkill::OnCooldownTimeUp,
			SkillCooldown, false);
		OnCooldownNotificationDelegate.Broadcast(GetRemainingCooldown(), SkillCooldown);
	}

}

bool AKRVSkill::IsSkillReady() const
{
	return !bIsInCooldown;

}

void AKRVSkill::OnCooldownTimeUp()
{
	OnCooldownNotificationDelegate.Broadcast(GetRemainingCooldown(), SkillCooldown);
	bIsInCooldown = false;

}

float AKRVSkill::GetRemainingCooldown()
{
	if (GetWorld())
	{
		return GetWorldTimerManager().GetTimerRemaining(CooldownTimer);
	}
	else
	{
		return -1.f;
	}

}

