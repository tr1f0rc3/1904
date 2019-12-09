// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "KRVEnums.h"
#include "KRVAIController.generated.h"

UCLASS()
class KRV_API AKRVAIController : public AAIController
{
	GENERATED_BODY()

public:
	AKRVAIController();

public:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION(BlueprintCallable)
		class AKRVCombatUnit* GetKRVCombatUnit();

	UFUNCTION(BlueprintCallable)
		class AKRVInGamePlayerController* GetPlayerControllerByTeam();

	UFUNCTION(BlueprintCallable)
		virtual void InitBBProperties();

	UFUNCTION()
		void OnSkillAcuqired(class AKRVSkill* Skill, class AKRVCombatUnit* ControlledUnit);

	UFUNCTION()
		void OnActionStatusChanged(EActionStatus NewStatus, class AKRVCombatUnit* ControlledUnit);

	UFUNCTION()
		void OnCombatStatusChanged(ECombatStatus NewStatus, class AKRVCombatUnit* ControlledUnit);

	UFUNCTION()
		void OnNearestEnemyRangeKeyChanged(ECombatStatus NewStatus, class AKRVCombatUnit* ControlledUnit);

	UFUNCTION()
		void OnTargetUnitChanged(class AKRVCombatUnit* TargetUnit, class AKRVCombatUnit* ControlledUnit);

	UFUNCTION()
		void OnTargetPointChanged(const FVector& NewPoint, class AKRVCombatUnit* ControlledUnit);

public:
	static const FName SelfIDKey;
	static const FName RangedMediumTypeKey;
	static const FName HasActiveSkillKey;
	static const FName ActionStatusKey;
	static const FName CombatStatusKey;
	static const FName NearestEnemyRangeKey;
	static const FName TargetUnitKey;
	static const FName TargetUnitIDKey;
	static const FName TargetPointKey;
	static const FName UnitToCheckKey;
	static const FName PointToCheckKey;

protected:
	UPROPERTY(BlueprintReadOnly)
		class UBehaviorTree* BTAsset;

	UPROPERTY(BlueprintReadOnly)
		class UBlackboardData* BBAsset;

	UPROPERTY(BlueprintReadOnly)
		class AKRVCombatUnit* KRVCombatUnit;


};
