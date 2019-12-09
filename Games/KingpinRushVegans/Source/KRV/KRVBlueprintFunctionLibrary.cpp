// Fill out your copyright notice in the Description page of Project Settings.

#include "KRVBlueprintFunctionLibrary.h"
#include "Runtime/Engine/Public/WorldCollision.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#include "Pawns/KRVCombatUnit.h"
#include "Pawns/KRVCharacter.h"
#include "Pawns/KRVTower.h"
#include "Controllers/KRVAIController.h"
#include "Controllers/KRVInGamePlayerController.h"
#include "Controllers/KRVMapPlayerController.h"
#include "Controllers/KRVPlayerAIController.h"
#include "Controllers/KRVEnemyAIController.h"

#include "KRVGameInstance.h"
#include "KRVGameState.h"
#include "PlayerStates/KRVInGamePlayerState.h"
#include "PlayerStates/KRVMapPlayerState.h"
#include "GameModes/KRVInGameGameMode.h"
#include "GameModes/KRVMapGameMode.h"

FGameplayTag UKRVBlueprintFunctionLibrary::GetGameplayTagByName(const FGameplayTagContainer& TagContainerToCheck, const FName& TagName)
{
	FGameplayTag NameCategoryTag = FGameplayTag::RequestGameplayTag(TagName);
	for (int32 i = 0; i < TagContainerToCheck.Num(); ++i)
	{
		if (TagContainerToCheck.GetByIndex(i).MatchesTag(NameCategoryTag))
		{
			return TagContainerToCheck.GetByIndex(i);
		}
	}
	return FGameplayTag();

}


bool UKRVBlueprintFunctionLibrary::GetGameplayTagsByName(FGameplayTagContainer& OutContainer, FGameplayTagContainer& ContainerToCheck, const FName& TagToGet)
{
	OutContainer.Reset(1);
	FGameplayTag NameCategoryTag = FGameplayTag::RequestGameplayTag(TagToGet);
	for (int32 i = 0; i < ContainerToCheck.Num(); ++i)
	{
		const FGameplayTag& TagToCheck = ContainerToCheck.GetByIndex(i);
		if (TagToCheck.MatchesTag(NameCategoryTag))
		{
			OutContainer.AddTag(TagToCheck);
		}
	}

	if (OutContainer.IsEmpty())
	{
		return false;
	}
	else
	{
		return true;
	}

}

bool UKRVBlueprintFunctionLibrary::MultiSpherePawnTrace(UObject* WorldContextObject, TArray<FHitResult>& OutHits, const FVector& StartLoc, const FVector& EndLoc, float Radius, bool bDoesDrawDebugLine)
{
	if (!WorldContextObject)
	{
		KRVCHECK(WorldContextObject);
		return false;
	}
	else
	{
		UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
		if (!World)
		{
			KRVCHECK(World);
			return false;
		}
		else
		{
			if (bDoesDrawDebugLine)
			{
				DrawDebugSphere(World, StartLoc, Radius, 32, FColor::Green, false, 2.0f);
			}

			World->SweepMultiByChannel(OutHits, StartLoc, EndLoc, FQuat::Identity,
				ECollisionChannel::ECC_Pawn,
				FCollisionShape::MakeSphere(Radius));

			TArray<AActor*> ActorsToCheckUnique;
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				const FHitResult& OutHit = OutHits[Index];
				if (ActorsToCheckUnique.Contains(OutHit.GetActor()))
				{
					OutHits.RemoveAt(Index, 1, false);
				}
				else
				{
					ActorsToCheckUnique.Emplace(OutHit.GetActor());
				}
			}
			OutHits.Shrink();

			if (OutHits.Num() > 0)
			{
				return true;
			}
		}
	}
	return false;

}

bool UKRVBlueprintFunctionLibrary::MultiRadialPawnTrace(UObject* WorldContextObject, TArray<FHitResult>& OutHits, const FVector& StartLoc, float Radius, bool bDoesDrawDebugLine /*= false*/)
{
	return MultiSpherePawnTrace(WorldContextObject, OutHits, StartLoc, StartLoc + 10000 * FVector::UpVector, Radius, bDoesDrawDebugLine);

}

bool UKRVBlueprintFunctionLibrary::SphereTargetTrace(const AActor* Target, FHitResult& OutHit, const FVector& StartLoc, const FVector& EndLoc, float Radius, bool bDoesDrawDebugLine)
{
	if (!Target)
	{
		KRVCHECK(Target);
	}
	else
	{
		TArray<FHitResult> Hits;
		MultiSpherePawnTrace(Target->GetWorld(), Hits, StartLoc, EndLoc, Radius, bDoesDrawDebugLine);
		for (FHitResult& Hit : Hits)
		{
			if (Target == Hit.GetActor())
			{
				OutHit = Hit;
				return true;
			}
		}
	}

	return false;
}

bool UKRVBlueprintFunctionLibrary::MultiRadialTeamTrace(UObject* WorldContextObject, TArray<FHitResult>& OutHits, const FVector& StartLoc, float Radius, EKRVTeam Team, bool bDoesDrawDebugLine)
{
	if (MultiRadialPawnTrace(WorldContextObject, OutHits, StartLoc, Radius, bDoesDrawDebugLine))
	{
		for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
		{
			AKRVCombatUnit* KRVCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
			if (KRVCombatUnit && KRVCombatUnit->GetTeam() == Team)
			{
				break;
			}
			OutHits.RemoveAt(Index, 1, false);
		}
		OutHits.Shrink();
		return true;
	}
	return false;

}


bool UKRVBlueprintFunctionLibrary::MultiRadialTrace(const AKRVCombatUnit* Lhs, TArray<FHitResult>& OutHits, const FVector& StartLoc, float Radius, ETargetType TargetType /*= ETargetType::AnyUnit*/, bool bDoesDrawDebugLine /*= false*/)
{
	OutHits.Empty();
	if (!Lhs)
	{
		KRVLOG(Error, TEXT("Invalid CombatUnit"));
	}
	else if (MultiRadialPawnTrace(Lhs->GetWorld(), OutHits, StartLoc, Radius, bDoesDrawDebugLine))
	{
		switch (TargetType)
		{
		case ETargetType::FriendlyCharacter:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVCharacter* HitCharacter = Cast<AKRVCharacter>(OutHits[Index].GetActor());
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (!HitCharacter || !IsFriendly(HitCombatUnit, Lhs) ||
					HitCombatUnit == Lhs || HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::NotFriendlyCharacter:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVCharacter* HitCharacter = Cast<AKRVCharacter>(OutHits[Index].GetActor());
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (HitCharacter && IsFriendly(HitCombatUnit, Lhs) ||
					HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::FriendlyTower:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVTower* HitTower = Cast<AKRVTower>(OutHits[Index].GetActor());
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (!HitTower || !IsFriendly(HitCombatUnit, Lhs) || HitCombatUnit == Lhs
					|| HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::AnyHostile:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (!HitCombatUnit || !IsHostile(HitCombatUnit, Lhs)
					|| HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::HostileCharacter:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVCharacter* HitCharacter = Cast<AKRVCharacter>(OutHits[Index].GetActor());
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (!HitCharacter || !IsHostile(HitCombatUnit, Lhs)
					|| HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::NotHostileCharacter:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVCharacter* HitCharacter = Cast<AKRVCharacter>(OutHits[Index].GetActor());
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (HitCharacter && (IsHostile(HitCombatUnit, Lhs) || HitCombatUnit->GetActionStatus() == EActionStatus::Dead))
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::HostileTower:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVTower* HitTower = Cast<AKRVTower>(OutHits[Index].GetActor());
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (!HitTower || !IsHostile(HitCombatUnit, Lhs)
					|| HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::AnyUnit:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (!HitCombatUnit || HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::AnyUnitNotSelf:
		{
			for (int32 Index = OutHits.Num() - 1; Index >= 0; --Index)
			{
				AKRVCombatUnit* HitCombatUnit = Cast<AKRVCombatUnit>(OutHits[Index].GetActor());
				if (!HitCombatUnit || Lhs == HitCombatUnit || HitCombatUnit->GetActionStatus() == EActionStatus::Dead)
				{
					OutHits.RemoveAt(Index, 1, false);
				}
			}
			break;
		}
		case ETargetType::AnyPawn:
		{
			break;
		}
		default:
		{
			OutHits.Empty();
			break;
		}
		}

		OutHits.Shrink();
		if (OutHits.Num() > 0)
		{
			return true;
		}
	}
	return false;

}

void UKRVBlueprintFunctionLibrary::MakeSquareFormation(TArray<FVector>& OutPositions, int32 Points, float UnitDistance)
{
	if (Points > 0)
	{
		OutPositions.Empty();

		int32 FormationColumns = FMath::CeilToInt(UKismetMathLibrary::Sqrt(Points));

		float PivotX = -1 * FMath::Abs(UnitDistance) * (FormationColumns - 1) / 2;
		float PivotY = PivotX;

		int32 IndexXY = 0;
		float VariationY = PivotY - UnitDistance;
		for (int32 IndexY = 0; IndexY < FormationColumns; ++IndexY)
		{
			VariationY += UnitDistance;
			float VariationX = PivotX - UnitDistance;
			for (int32 IndexX = 0; IndexX < FormationColumns && IndexXY < Points; ++IndexX, ++IndexXY)
			{
				VariationX += UnitDistance;
				OutPositions.Emplace(FVector(VariationX, VariationY, 0.f));
			}
		}
	}

}

bool UKRVBlueprintFunctionLibrary::IsSuitableTarget(const AKRVCombatUnit* TargetedUnit, const AKRVCombatUnit* TargetingUnit, ETargetType TargetType)
{
	if (!TargetedUnit || !TargetingUnit)
	{
		KRVCHECK(TargetingUnit);
		KRVCHECK(TargetedUnit);
	}
	else if (TargetType == ETargetType::AnyUnit)
	{
		return true;
	}
	else if (Cast<AKRVCharacter>(TargetedUnit))
	{
		if ((IsFriendly(TargetingUnit, TargetedUnit) && TargetType != ETargetType::FriendlyCharacter) ||
			(IsHostile(TargetingUnit, TargetedUnit) && TargetType != ETargetType::HostileCharacter))
		{
			// not implemented;
		}
		else
		{
			return true;
		}
	}
	else if (Cast<AKRVTower>(TargetedUnit))
	{
		if ((IsFriendly(TargetingUnit, TargetedUnit) && TargetType != ETargetType::FriendlyTower) ||
			(IsHostile(TargetingUnit, TargetedUnit) && TargetType != ETargetType::HostileTower))
		{
			KRVLOG(Warning, TEXT("TargetType is unmatched"));
		}
		else
		{
			return true;
		}
	}
	return false;

}

bool UKRVBlueprintFunctionLibrary::IsFriendly(const AKRVCombatUnit* Lhs, const AKRVCombatUnit* Rhs)
{
	if (!Lhs || !Rhs)
	{
		KRVLOG(Error, TEXT("Invalid CombatUnit"));
	}
	else
	{
		return Lhs->GetTeam() == Rhs->GetTeam();
	}
	return false;

}

bool UKRVBlueprintFunctionLibrary::IsHostile(const AKRVCombatUnit* Lhs, const AKRVCombatUnit* Rhs)
{
	if (!Lhs || !Rhs)
	{
		KRVLOG(Error, TEXT("Invalid CombatUnit"));
	}
	else
	{
		return Lhs->GetTeam() != Rhs->GetTeam();
	}
	return false;

}

UKRVGameInstance* UKRVBlueprintFunctionLibrary::GetKRVGameInstance(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		KRVCHECK(World);
	}
	else
	{
		return Cast<UKRVGameInstance>(UGameplayStatics::GetGameInstance(World));
	}
	return nullptr;

}

AKRVInGameGameMode* UKRVBlueprintFunctionLibrary::GetKRVInGameGameMode(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		KRVCHECK(World);
	}
	else
	{
		return Cast<AKRVInGameGameMode>(World->GetAuthGameMode());
	}
	return nullptr;

}

AKRVMapGameMode* UKRVBlueprintFunctionLibrary::GetKRVMapGameMode(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		KRVCHECK(World);
	}
	else
	{
		return Cast<AKRVMapGameMode>(World->GetAuthGameMode());
	}
	return nullptr;

}

AKRVGameState* UKRVBlueprintFunctionLibrary::GetKRVGameState(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		KRVCHECK(World);
	}
	else
	{
		return Cast<AKRVGameState>(UGameplayStatics::GetGameState(World));
	}
	return nullptr;

}

AKRVPlayerController* UKRVBlueprintFunctionLibrary::GetKRVPlayerController(UObject* WorldContextObject)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	if (!World)
	{
		KRVCHECK(World);
	}
	else
	{
		UKRVGameInstance* KRVGameInstance = UKRVBlueprintFunctionLibrary::GetKRVGameInstance(WorldContextObject);
		if (!KRVGameInstance)
		{
			KRVCHECK(KRVGameInstance);
		}
		else
		{
			return Cast<AKRVPlayerController>
				(KRVGameInstance->GetEngine()->GetFirstLocalPlayerController(World));
		}
	}
	return nullptr;


}

AKRVInGamePlayerController* UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(UObject* WorldContextObject)
{
	return Cast<AKRVInGamePlayerController>(GetKRVPlayerController(WorldContextObject));

}

AKRVMapPlayerController* UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(UObject* WorldContextObject)
{
	return Cast<AKRVMapPlayerController>(GetKRVPlayerController(WorldContextObject));

}

AKRVInGamePlayerState* UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerState(UObject* WorldContextObject)
{
	AKRVInGamePlayerController* KRVInGamePlayerController = UKRVBlueprintFunctionLibrary::GetKRVInGamePlayerController(WorldContextObject);
	if (!KRVInGamePlayerController)
	{
		KRVCHECK(KRVInGamePlayerController);
	}
	else
	{
		return KRVInGamePlayerController->GetPlayerState<AKRVInGamePlayerState>();
	}
	return nullptr;

}

AKRVMapPlayerState* UKRVBlueprintFunctionLibrary::GetKRVMapPlayerState(UObject* WorldContextObject)
{
	AKRVMapPlayerController* KRVMapPlayerController = UKRVBlueprintFunctionLibrary::GetKRVMapPlayerController(WorldContextObject);
	if (!KRVMapPlayerController)
	{
		KRVCHECK(KRVMapPlayerController);
	}
	else
	{
		return KRVMapPlayerController->GetPlayerState<AKRVMapPlayerState>();
	}
	return nullptr;

}

void UKRVBlueprintFunctionLibrary::GetStringFromSeconds(FString& OutString, int32 NewSeconds)
{
	int32 SecondsToShow = (60 + NewSeconds) % 60;
	int32 MinutesToShow = NewSeconds / 60;

	FString SecondsString;
	if (SecondsToShow < 10)
	{
		SecondsString = FString(TEXT("0"));
	}
	SecondsString += FString::FromInt(SecondsToShow);

	OutString = FString::FromInt(MinutesToShow) + ":" + SecondsString;

}

