// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVEnums.h"
#include "KRVGameResultUserWidget.generated.h"

UCLASS()
class KRV_API UKRVGameResultUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

public:
	bool BindToGameState();

	UFUNCTION(BlueprintNativeEvent)
		void ShowThisWidget();
	void ShowThisWidget_Implementation();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
		void ShowMenuWidget();
	void ShowMenuWidget_Implementation();

protected:
	UPROPERTY(BlueprintReadOnly)
		EGameState GameStateEnum;

protected: // Victory
	UFUNCTION(BlueprintCallable)
		bool IsLargerOrEqualThanEarnedStars(int32 StarsToCheck) const;

protected:
	UPROPERTY(BlueprintReadOnly)
		class UVerticalBox* VictoryVerticalBox;

	UPROPERTY(BlueprintReadOnly)
		class USizeBox* StarSizeBox_1;

	UPROPERTY(BlueprintReadOnly)
		class USizeBox* StarSizeBox_2;

	UPROPERTY(BlueprintReadOnly)
		class USizeBox* StarSizeBox_3;

	int32 EarnedStars = 0;

protected: // Defeat
	UPROPERTY(BlueprintReadOnly)
		class UVerticalBox* DefeatVerticalBox;

protected: // record
	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* ClearTimeTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* RubyTextBlock;

	UPROPERTY(BlueprintReadWrite, Transient)
		bool bIsRubyCounting;

	int32 EarnedRuby = 0;
	float RubyToShow = 0.f;

protected: // MenuWidget
	UPROPERTY(BlueprintReadOnly)
		class UKRVInGameMenuUserWidget* MenuWidgets;

};
