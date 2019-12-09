// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVConfirmUserWidget.generated.h"

DECLARE_DELEGATE(FOnConfirmClickedDelegate);

UCLASS()
class KRV_API UKRVConfirmUserWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;

public:
	virtual bool SetConfirmText(const FText& NewText);

protected:
	UFUNCTION()
		void OnConfirmIconClicked();

	UFUNCTION()
		void OnCancleIconClicked();

public:
	FOnConfirmClickedDelegate OnConfirmClickedDelegate;

protected:
	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* ConfirmIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UKRVIconUserWidget* CancleIconWidget;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* ConfirmTextBlock;

};
