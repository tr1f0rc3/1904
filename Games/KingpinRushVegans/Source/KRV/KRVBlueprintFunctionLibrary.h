// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayTagContainer.h"

#include "KRVEnums.h"
#include "KRV.h"

#include "KRVBlueprintFunctionLibrary.generated.h"

class AKRVCombatUnit;

UCLASS()
class KRV_API UKRVBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public: // Array
	template <typename T1, typename T2>
	static FORCEINLINE void SetArraySizeSame(TArray<T1>& ArrayToChange, TArray<T2>& PivotArray, bool bShouldSizeSameBeforeCheck = false)
	{
		if (ArrayToChange.Num() != PivotArray.Num())
		{
			if (bShouldSizeSameBeforeCheck)
			{
				KRVCHECK(ArrayToChange.Num() == PivotArray.Num());
			}

			int32 ArrayNumDifference = FMath::Abs(ArrayToChange.Num() - PivotArray.Num());
			if (ArrayToChange.Num() > PivotArray.Num())
			{
				ArrayToChange.RemoveAt(PivotArray.Num(), ArrayNumDifference, true);
			}
			else if (ArrayToChange.Num() < PivotArray.Num())
			{
				for (int32 Index = 0; Index < ArrayNumDifference; ++Index)
				{
					ArrayToChange.Emplace(T1());
				}
			}
		}
	}

public: // Tag
	UFUNCTION(BlueprintCallable)
		static FGameplayTag GetGameplayTagByName(const FGameplayTagContainer& TagContainerToCheck, const FName& TagName);

	UFUNCTION(BlueprintCallable)
		static bool GetGameplayTagsByName(FGameplayTagContainer& OutContainer, FGameplayTagContainer& ContainerToCheck, const FName& TagToGet);

public: // Trace
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static bool MultiSpherePawnTrace(UObject* WorldContextObject, TArray<FHitResult>& OutHits,
			const FVector& StartLoc, const FVector& EndLoc, float Radius, bool bDoesDrawDebugLine = false);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static bool MultiRadialPawnTrace(UObject* WorldContextObject, TArray<FHitResult>& OutHits,
			const FVector& StartLoc, float Radius, bool bDoesDrawDebugLine = false);

	UFUNCTION(BlueprintCallable)
		static bool SphereTargetTrace(const AActor* Target, FHitResult& OutHit,
			const FVector& StartLoc, const FVector& EndLoc, float Radius, bool bDoesDrawDebugLine = false);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static bool MultiRadialTeamTrace(UObject* WorldContextObject, TArray<FHitResult>& OutHits,
			const FVector& StartLoc, float Radius, EKRVTeam Team, bool bDoesDrawDebugLine = false);

	UFUNCTION(BlueprintCallable)
		static bool MultiRadialTrace(const AKRVCombatUnit* Lhs, TArray<FHitResult>& OutHits,
			const FVector& StartLoc, float Radius, ETargetType TargetType = ETargetType::AnyPawn, bool bDoesDrawDebugLine = false);


	template <class T = AActor>
	static FORCEINLINE bool MultiRadialTrace_Template(const AKRVCombatUnit* Lhs, TArray<T*>& OutActors,
		const FVector& StartLoc, float Radius, ETargetType TargetType = ETargetType::AnyPawn, bool bDoesDrawDebugLine = false)
	{
		OutActors.Empty();
		TArray<FHitResult> OutHits;
		MultiRadialTrace(Lhs, OutHits, StartLoc, Radius, TargetType, bDoesDrawDebugLine);
		for (const FHitResult& OutHit : OutHits)
		{
			T* OutActor = CastChecked<T>(OutHit.GetActor());
			if (OutActor)
			{
				OutActors.Emplace(OutActor);
			}
		}
		if (OutActors.Num() > 0)
		{
			return true;
		}
		return false;
	}

public: // Location
	UFUNCTION(BlueprintCallable)
		static void MakeSquareFormation(TArray<FVector>& OutPositions, int32 Points, float UnitDistance);

public: // Targeting 
	UFUNCTION(BlueprintCallable)
		static bool IsSuitableTarget(const AKRVCombatUnit* TargetedUnit, const AKRVCombatUnit* TargetingUnit, ETargetType TargetType);

	UFUNCTION(BlueprintCallable)
		static bool IsHostile(const AKRVCombatUnit* Lhs, const AKRVCombatUnit* Rhs);

	UFUNCTION(BlueprintCallable)
		static bool IsFriendly(const AKRVCombatUnit* Lhs, const AKRVCombatUnit* Rhs);


public: // GameState, GameMode, GameState, PlayerState
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class UKRVGameInstance* GetKRVGameInstance(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVInGameGameMode* GetKRVInGameGameMode(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVMapGameMode* GetKRVMapGameMode(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVGameState* GetKRVGameState(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVPlayerController* GetKRVPlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVInGamePlayerController* GetKRVInGamePlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVMapPlayerController* GetKRVMapPlayerController(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVInGamePlayerState* GetKRVInGamePlayerState(UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
		static class AKRVMapPlayerState* GetKRVMapPlayerState(UObject* WorldContextObject);

public:	// time
	UFUNCTION(BlueprintCallable)
		static void GetStringFromSeconds(FString& OutString, int32 Seconds);


};
