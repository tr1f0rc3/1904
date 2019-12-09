// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "KRVLogUserWidget.generated.h"

DECLARE_DELEGATE_OneParam(FOnThisWidgetHiddenDelegate, class UKRVLogUserWidget*);

UCLASS()
class KRV_API UKRVLogUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

public:
	bool PopupLog(const FText& NewText, class UTexture2D* NewTexture = nullptr);

protected:
	UFUNCTION(BlueprintNativeEvent)
		void ShowThisWidget();
	void ShowThisWidget_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void HideThisWidget();
	void HideThisWidget_Implementation();

	void SetSizeBoxAuto();
	void SetLogText(const FText& NewText, class UTexture2D* NewTexture = nullptr);
	bool SetLogIconImage(UTexture2D* NewTexture = nullptr);

public:
	int32 LogSlotIndex;
	FOnThisWidgetHiddenDelegate OnThisWidgetHiddenDelegate;

protected:
	UPROPERTY(BlueprintReadOnly)
		class USizeBox* MainSizeBox;

	UPROPERTY(BlueprintReadOnly)
		class UTextBlock* LogTextBlock;

	UPROPERTY(BlueprintReadOnly)
		class UImage* LogIconImage;

	UPROPERTY(BlueprintReadWrite)
		float HideDelay = 3.f;

	FTimerHandle HideTimerHandle;

};
