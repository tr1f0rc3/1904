// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ICursor.h"
#include "GameplayTagContainer.h"

#include "KRVEnums.h"
#include "Controllers/KRVPlayerController.h"
#include "KRVInGamePlayerController.generated.h"

class UKRVScoreComponent;
class AKRVInGamePlayerState;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnTargetingBeginDelegate, ETargetingType);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnUnitSelectedDelegate, bool, AKRVCombatUnit*);
DECLARE_MULTICAST_DELEGATE(FOnTargetingEndMultiDelegate);
DECLARE_DELEGATE(FOnTargetingEndDelegate)

UENUM(BlueprintType)
enum class EClickToDo : uint8
{
	NONE,
	SelectUnit,
	SetWaypoint,
	SetTargetOfTargeter,
	MoveHero,
	MAX,

};

UCLASS()
class KRV_API AKRVInGamePlayerController : public AKRVPlayerController
{
	GENERATED_BODY()

public:
	AKRVInGamePlayerController();

public:
	virtual void PostInitializeComponents() override;
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;

protected:
	virtual void BeginPlay() override;

public: // Level
	UFUNCTION(BlueprintCallable)
		void RequestToStartGame();

	UFUNCTION(BlueprintCallable)
		void ReturnToMainMenu();

	UFUNCTION()
		void OnGameStoppedAfterGameEnd();

protected:
	UFUNCTION()
		void StartGameAfterFadeIn();

	UFUNCTION()
		void ReturnToMainMenuAfterFadeOut();

protected:
	UFUNCTION(BlueprintCallable)
		void OnLMBClicked();

	virtual void SetMouseCursorInTick() override;

public: // UI
	bool BindHeroToWidget(class AKRVHero* NewHero);
	bool BindCommanderToWidget(class AKRVCombatUnit* NewCommander);

protected:
	UFUNCTION(BlueprintCallable)
		bool ShowInGameMainWidget(bool bIsTrue);

	UFUNCTION(BlueprintCallable)
		bool ShowGameResultWidget();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class TSubclassOf<class UKRVInGameMainUserWidget> InGameMainWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UKRVInGameMainUserWidget* InGameMainWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class TSubclassOf<UUserWidget> InGameResultWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class TSubclassOf<class UKRVGameResultUserWidget> GameResultWidgetClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
		class UKRVGameResultUserWidget* GameResultWidget;

public: // Unit Control
	UFUNCTION(BlueprintCallable)
		class AKRVCombatUnit* GetCurrentSelectedUnit() const;

	UFUNCTION(BlueprintCallable)
		bool SelectUnit(class AKRVCombatUnit* KRVCombatUnit);

	UFUNCTION(BlueprintCallable)
		bool UnselectUnit();

	UFUNCTION(BlueprintCallable)
		bool CancleTargeting();

protected:
	AKRVCombatUnit* GetUnitUnderCursor() const;
	bool GetPointUnderCursor(FVector& OutVector);
	bool ClickStaticUnderCursor();
	bool SelectUnitUnderCursor();

	void PlayConfirmToLocationEffect(const FVector& NewLocation);

public:
	FOnTargetingBeginDelegate OnTargetingBeginDelegate;
	FOnTargetingEndMultiDelegate OnTargetingEndMultiDelegate;
	FOnTargetingEndDelegate OnTargetingEndDelegate;
	FOnUnitSelectedDelegate OnUnitSelectedDelegate;

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		EClickToDo ClickToDo = EClickToDo::SelectUnit;

	UPROPERTY(Transient, VisibleAnywhere, BlueprintReadOnly)
		TWeakObjectPtr<class AKRVCombatUnit> CurrentSelectedUnit;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystem* OnStaticClickedParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class UParticleSystem* ConfirmToLocationParticle;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		class USoundCue* ConfirmToLocationSound;

protected: // Move
	bool MoveHeroToLocation();
	bool SetRecruitWaypoint();

public: // Skill
	UFUNCTION()
		bool TryCastSkill(const FGameplayTag& SkillTagToCast, class AKRVCombatUnit* Caster);

	UFUNCTION(BlueprintCallable)
		bool IsTargeting() const;

	UFUNCTION(BlueprintCallable)
		AKRVCombatUnit* GetReservedTargeter() const;

protected:
	bool BeginTargeting(ETargetingType NewWhereToCastSkill);
	bool MakeTargeterCastSkillToUnitUnderCursor();
	bool MakeTargeterCastSkillToPointUnderCursor();


protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		ETargetingType TargetingType;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		TWeakObjectPtr<class AKRVCombatUnit> ReservedTargeter;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		FGameplayTag ReservedSkillTagToCast;

	FTimerHandle SkillReserveTimer;

};
