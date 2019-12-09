// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "KRVKillRewardUserWidget.generated.h"

UCLASS()
class KRV_API UKRVKillRewardUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeOnInitialized() override;

protected:
	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* ExpTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UHorizontalBox* ExpHorizontalBox;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* GoldTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UHorizontalBox* GoldHorizontalBox;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* RubyTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UHorizontalBox* RubyHorizontalBox;

public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void PopUpThisWidget();

	bool BindToCombatUnit(class AKRVCombatUnit* NewUnit);

protected:
	UFUNCTION(BlueprintCallable)
		void HideThisWidget();

	UFUNCTION()
		void OnUnitDieWithReward(int32 NewGold, int32 NewExp, int32 NewRuby, class AKRVCombatUnit* NewUnit);

	void SetTextAndVisibilityOfTheBoxes(int32 NewExperience, int32 NewGold, int32 NewRuby);
	void IntToPlusMinusText(FText& OutText, int32 NewValue);

};
