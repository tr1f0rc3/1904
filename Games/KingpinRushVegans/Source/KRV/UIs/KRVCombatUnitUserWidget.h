// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVCombatUnitUserWidget.generated.h"

UCLASS()
class KRV_API UKRVCombatUnitUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

public:
	bool BindToCharacter(class AKRVCharacter* NewCharacter);
	bool BindToCombatUnit(class AKRVCombatUnit* NewCombatUnit);

protected:
	UFUNCTION()
		void ShowHPBarWidget(bool bIsTrue);

	UFUNCTION()
		void ShowCastingBarWidget(bool bIsTrue);

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVPBUserWidget* HPBarWidget;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly)
		class UKRVPBUserWidget* CastingBarWidget;

};
