// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVPBUserWidget.generated.h"

DECLARE_DELEGATE_OneParam(FNeedToShowDelegate, bool)

UENUM(BlueprintType)
enum class EPBType : uint8
{
	NONE,
	HP,
	Exp,
	Casting,
	MAX

};


UCLASS()
class KRV_API UKRVPBUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		EPBType PBType;

	UPROPERTY()
		class UProgressBar* ProgressBar;

public:
	FNeedToShowDelegate NeedToShowDelegate;

public: // HP
	UFUNCTION(BlueprintCallable)
		bool BindToHealth(class UKRVCharacterStatComponent* NewComponent);

protected:
	UFUNCTION()
		void OnHPChanged(float CurrentHealth, float MaxHealth);

public: // Exp
	UFUNCTION(BlueprintCallable)
		bool BindToExp(class AKRVInGamePlayerState* NewPlayerState = nullptr);

protected:
	UFUNCTION()
		void OnExpChanged(int32 NewExp, int32 ExpToLevelUp);

public: // Cast
	UFUNCTION(BlueprintCallable)
		bool BindToCasting(class AKRVCombatUnit* NewCaster);

protected:
	UFUNCTION()
		void OnCastingBegin(class AKRVSkill* NewSkill, class AKRVCombatUnit* NewCaster);

	UFUNCTION()
		void OnCastingCancle(class AKRVSkill* NewSkill, class AKRVCombatUnit* NewCaster);

	UFUNCTION()
		void OnCastingEnd(class AKRVSkill* NewSkil, class AKRVCombatUnit* NewCaster);

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnCastingBeginEffect();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnCastingCancleEffect();

	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent)
		void OnCastingEndEffect();

protected:
	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVCombatUnit* BoundCaster;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		class AKRVSkill* BoundSkill;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		bool bIsCasting;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		float ElapsedCastingTime;

	UPROPERTY(Transient, VisibleInstanceOnly, BlueprintReadOnly)
		float TotalCastingTime;


};
